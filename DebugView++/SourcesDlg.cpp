// (C) Copyright Gert-Jan de Vos and Jan Wilmans 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// Repository at: https://github.com/djeedjay/DebugViewPP/

#include "stdafx.h"
#include <boost/algorithm/string.hpp>
#include <atlstr.h>
#include "Win32Lib/utilities.h"
#include "DebugView++Lib/LogFilter.h"
#include "DebugView++Lib/LogSource.h"
#include "Resource.h"
#include "SourceDlg.h"
#include "SourcesDlg.h"

namespace fusion {
namespace debugviewpp {

BEGIN_MSG_MAP_TRY(CSourcesDlg)
	MSG_WM_INITDIALOG(OnInitDialog)
	NOTIFY_CODE_HANDLER_EX(PIN_CLICK, OnClickItem)
	COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	COMMAND_ID_HANDLER_EX(IDOK, OnOk)
	COMMAND_ID_HANDLER_EX(IDADD, OnAdd)
	REFLECT_NOTIFICATIONS()
	CHAIN_MSG_MAP(CDialogResize<CSourcesDlg>)
END_MSG_MAP_CATCH(ExceptionHandler)

CSourcesDlg::CSourcesDlg(std::vector<SourceInfo> sourceInfos) : m_sourceInfos(sourceInfos)
{
}

void CSourcesDlg::ExceptionHandler()
{
	MessageBox(WStr(GetExceptionMessage()).c_str(), LoadString(IDR_APPNAME).c_str(), MB_ICONERROR | MB_OK);
}

void CSourcesDlg::UpdateGrid()
{
	m_grid.DeleteAllItems();
	for (auto it= m_sourceInfos.begin(); it != m_sourceInfos.end(); ++it)
	{
		auto& info = (*it);
		auto typeName = WStr(SourceTypeToString(info.type));

		int item = m_grid.GetItemCount();
		m_grid.InsertItem(item, PropCreateCheckButton(L"", info.enabled));
		m_grid.SetSubItem(item, 1, PropCreateReadOnlyItem(L"", info.description.c_str()));
		m_grid.SetSubItem(item, 2, PropCreateReadOnlyItem(L"", typeName.c_str()));
		if (info.type == SourceType::System)
			m_grid.SetSubItem(item, 3, PropCreateReadOnlyItem(L"", L""));
		else
			m_grid.SetSubItem(item, 3, PropCreateReadOnlyItem(L"", L"�"));
	}
}

BOOL CSourcesDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	m_grid.SubclassWindow(GetDlgItem(IDC_SOURCES_GRID));
	m_grid.InsertColumn(0, L"", LVCFMT_LEFT, 32, 0);
	m_grid.InsertColumn(1, L"Source description", LVCFMT_LEFT, 280, 0);
	m_grid.InsertColumn(2, L"Type", LVCFMT_LEFT, 100, 0);
	m_grid.InsertColumn(3, L"", LVCFMT_LEFT, 16, 0);
	m_grid.SetExtendedGridStyle(PGS_EX_SINGLECLICKEDIT);

	UpdateGrid();
	CenterWindow(GetParent());
	DlgResize_Init();
	return TRUE;
}

LRESULT CSourcesDlg::OnClickItem(NMHDR* pnmh)
{
	auto& nmhdr = *reinterpret_cast<NMPROPERTYITEM*>(pnmh);

	int iItem = -1;
	int iSubItem = -1;
	if (m_grid.FindProperty(nmhdr.prop, iItem, iSubItem) && iSubItem == 3)
	{
		if (GetSourceType(iItem) != SourceType::System)
		{
			m_grid.DeleteItem(iItem);
			m_sourceInfos[iItem].remove = true;
			return TRUE;
		}
	}
	return FALSE;
}

bool CSourcesDlg::GetSourceEnable(int iItem) const
{
	CComVariant val;
	GetGridItem<CPropertyCheckButtonItem>(m_grid, iItem, 0).GetValue(&val);
	return val.boolVal != VARIANT_FALSE;
}

std::wstring CSourcesDlg::GetSourceText(int iItem) const
{
	return GetGridItemText(m_grid, iItem, 1);
}

SourceType::type CSourcesDlg::GetSourceType(int iItem) const
{
	return StringToSourceType(Str(GetGridItemText(m_grid, iItem, 2)));
}


void CSourcesDlg::OnCancel(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	m_sourceInfos.clear();
	EndDialog(nID);
}

void CSourcesDlg::OnOk(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	EndDialog(nID);
}

void CSourcesDlg::OnAdd(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	CSourceDlg dlg;
	if (dlg.DoModal() != IDOK)
		return;

	auto info = SourceInfo(dlg.GetName(), dlg.GetSourceType(), dlg.GetAddress(), dlg.GetPort());
	info.enabled = true;
	m_sourceInfos.push_back(info);
	UpdateGrid();
}

std::vector<SourceInfo> CSourcesDlg::GetSourceInfos()
{
	return m_sourceInfos;
}

} // namespace debugviewpp 
} // namespace fusion