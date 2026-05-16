#include "detour.hpp"

#include "utils/utils.hpp"
#include <MinHook.h>

namespace
{
	void** get_iat_entry(const char* module_name, const char* import_module_name, const char* proc_name)
	{
		const auto module_base = reinterpret_cast<uintptr_t>(GetModuleHandleA(module_name));
		if (module_base == 0) return nullptr;

		const auto dos_headers = reinterpret_cast<PIMAGE_DOS_HEADER>(module_base);
		const auto nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(module_base + dos_headers->e_lfanew);

		for (auto import_descriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(module_base + nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress); import_descriptor->Name; ++import_descriptor) {
			if (_stricmp(reinterpret_cast<const char*>(module_base + import_descriptor->Name), import_module_name) != 0) continue;

			auto original_first_thunk = reinterpret_cast<PIMAGE_THUNK_DATA>(module_base + import_descriptor->OriginalFirstThunk);
			auto first_thunk = reinterpret_cast<PIMAGE_THUNK_DATA>(module_base + import_descriptor->FirstThunk);
			for (; original_first_thunk->u1.AddressOfData; ++original_first_thunk, ++first_thunk) {
				if (IMAGE_SNAP_BY_ORDINAL(original_first_thunk->u1.Ordinal)) continue;
				if (_stricmp(reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(module_base + original_first_thunk->u1.AddressOfData)->Name, proc_name) != 0) continue;
				return reinterpret_cast<void**>(&first_thunk->u1.Function);
			}
		}

		return nullptr;
	}
}

void detour::initialize()
{
	if (MH_Initialize() != MH_OK) {
		utils::fatal_error("Failed to initialize MinHook.");
	}
}

void detour::uninitialize()
{
	MH_Uninitialize();
}

detour::~detour()
{
	this->remove();
}

void detour::create(void* target, void* hook)
{
	this->remove();
	this->target_ = target;

	if (MH_CreateHook(this->target_, hook, &this->original_) != MH_OK) {
		utils::fatal_error("Unable to create hook at location: %p", this->target_);
	}

	this->enable();
}

void detour::create(const uintptr_t address, void* hook)
{
	this->create(reinterpret_cast<void*>(address), hook);
}

void detour::create(const wchar_t* module_name, const char* proc_name, void* hook)
{
	this->remove();

	if (MH_CreateHookApiEx(module_name, proc_name, hook, &this->original_, &this->target_) != MH_OK) {
		utils::fatal_error("Unable to create hook for function %s in %ls at location: %p", proc_name, module_name, this->target_);
	}

	this->enable();
}

void detour::remove()
{
	if (this->target_) {
		MH_DisableHook(this->target_);
		MH_RemoveHook(this->target_);
		this->target_ = nullptr;
		this->original_ = nullptr;
	}
	if (this->iat_target_) {
		this->remove_iat();
	}
}

void detour::enable() const
{
	if (this->target_) {
		if (MH_EnableHook(this->target_) != MH_OK) {
			utils::fatal_error("Unable to enable hook at location: %p", this->target_);
		}
		return;
	}
	if (this->iat_target_) {
		this->enable_iat();
	}
}

void detour::disable() const
{
	if (this->target_) {
		if (MH_DisableHook(this->target_) != MH_OK) {
			utils::fatal_error("Unable to disable hook at location: %p", this->target_);
		}
		return;
	}
	if (this->iat_target_) {
		this->disable_iat();
	}
}

void detour::create_iat(const char* module_name, const char* import_module_name, const char* proc_name, void* hook)
{
	this->remove();

	this->iat_target_ = get_iat_entry(module_name, import_module_name, proc_name);
	if (this->iat_target_ == nullptr) {
		utils::fatal_error("Unable to get IAT entry address for function %s in %s", proc_name, import_module_name);
	}

	this->original_ = *this->iat_target_;
	this->iat_detour_ = hook;

	this->enable_iat();
}

void detour::remove_iat()
{
	this->disable_iat();

	this->iat_detour_ = nullptr;
	this->original_ = nullptr;
	this->iat_target_ = nullptr;
}

void detour::enable_iat() const
{
	if (!this->iat_target_) return;

	DWORD old_protect;
	if (!VirtualProtect(this->iat_target_, sizeof this->iat_detour_, PAGE_READWRITE, &old_protect)) {
		utils::fatal_error("Unable to enable hook at location: %p", this->iat_target_);
	}
	*this->iat_target_ = this->iat_detour_;
	VirtualProtect(this->iat_target_, sizeof this->iat_detour_, old_protect, &old_protect);
}

void detour::disable_iat() const
{
	if (!this->iat_target_) return;

	DWORD old_protect;
	if (!VirtualProtect(this->iat_target_, sizeof this->iat_detour_, PAGE_READWRITE, &old_protect)) {
		utils::fatal_error("Unable to disable hook at location: %p", this->iat_target_);
	}
	*this->iat_target_ = this->original_;
	VirtualProtect(this->iat_target_, sizeof this->iat_detour_, old_protect, &old_protect);
}

void* detour::get_target() const
{
	return this->iat_target_ != nullptr ? this->iat_target_ : this->target_;
}

void* detour::get_target(const char* module_name, const char* proc_name)
{
	const HMODULE h_module = GetModuleHandleA(module_name);
	if (h_module == nullptr) {
		utils::fatal_error("Unable to get module handle for %s", module_name);
	}

	this->target_ = GetProcAddress(h_module, proc_name);
	if (this->target_ == nullptr) {
		utils::fatal_error("Unable to get address for function %s in %s", proc_name, module_name);
	}

	return this->target_;
}

void* detour::get_target(const char* module_name, const char* import_module_name, const char* proc_name)
{
	this->iat_target_ = get_iat_entry(module_name, import_module_name, proc_name);
	if (this->iat_target_ == nullptr) {
		utils::fatal_error("Unable to get IAT entry address for function %s in %s", proc_name, import_module_name);
	}

	return this->iat_target_;
}
