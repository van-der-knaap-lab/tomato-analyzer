/* This file is part of Tomato Analyzer.

    Tomato Analyzer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tomato Analyzer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Tomato Analyzer.  If not, see <http://www.gnu.org/licenses/>. */
LRESULT CALLBACK DataSaveDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int	nDataNumber, nSrcIndex, nIndex, nDestIndex, iloop, nFormatType;
	DWORD	dwData;
	HWND	hwnd;
	int		nFormatAll[MAX_DATA_FORMAT];
	CHAR	szFormat[32];
	CCoordinator *	pCod;
	CTrimbleGPSReceiver * pCgps;
	static int	nMovedLast=-1;

	if(message==mDragListMsg)
	{
		DRAGLISTINFO *	pdgi;

		pdgi=(LPDRAGLISTINFO)lParam;
		hwnd=GetDlgItem(hDlg, wParam);

		if(pdgi->uNotification!=DL_BEGINDRAG)
		{
			dwData=1;
		}

		switch(pdgi->uNotification)
		{
			case DL_BEGINDRAG:
				 nMovedLast=-1;
				 nIndex=LBItemFromPt(hwnd, pdgi->ptCursor,TRUE);

				 if(nMovedLast != nIndex)
				 {
					DrawDragLegend(hwnd, nMovedLast);
					DrawDragLegend(hwnd, nIndex);

					nMovedLast = nIndex;
				 }
				 return TRUE;

			case DL_DRAGGING:
				 nIndex=LBItemFromPt(hwnd, pdgi->ptCursor,FALSE);
				 if(nMovedLast != nIndex)
				 {
					DrawDragLegend(hwnd, nMovedLast);
					DrawDragLegend(hwnd, nIndex);

					nMovedLast = nIndex;
				 }

				 LBItemFromPt(hwnd, pdgi->ptCursor,TRUE);
				 return TRUE; //(nIndex == LB_ERR) ? DL_STOPCURSOR : DL_MOVECURSOR;

			case DL_DROPPED:
				 DrawDragLegend(hwnd, nMovedLast);
				 
				 nSrcIndex = SendMessage(hwnd, LB_GETCURSEL, 0, 0);
				 nDestIndex = LBItemFromPt(hwnd, pdgi->ptCursor,TRUE);
				 
				 if(nSrcIndex == -1 || nDestIndex == -1)
					 return 0;
				 if(nDestIndex == nSrcIndex || nDestIndex == nSrcIndex+1)
					 return 0;

				 SendMessage(hwnd, LB_GETTEXT, nSrcIndex, (LPARAM)szFormat);
				 dwData=SendMessage(hwnd, LB_GETITEMDATA, nSrcIndex, 0);
				 SendMessage(hwnd, LB_DELETESTRING, nSrcIndex, 0);

				 if(nSrcIndex < nDestIndex)
					 nDestIndex--;
				 nDestIndex = SendMessage(hwnd, LB_INSERTSTRING, nDestIndex, (LPARAM)(LPVOID)szFormat);
				 SendMessage(hwnd, LB_SETITEMDATA, nDestIndex, dwData);
				 SendMessage(hwnd, LB_SETCURSEL, nDestIndex, 0);
				 return TRUE;

			case DL_CANCELDRAG:
				 DrawDragLegend(hwnd, nMovedLast);
				 return TRUE;
		}
	}

	switch(message)
	{
		case WM_INITDIALOG:
			 SetWindowLong(hDlg, GWL_USERDATA, lParam);
             
			 pCod=(CCoordinator *)lParam;
			 pCgps=pCod->GetGPSreceiver(0);

			 hwnd=GetDlgItem(hDlg, IDC_FROM_DATA);

			 nDataNumber=pCgps->GetDataFormatNum(ID_NOTUSED);
			 for(iloop=0; iloop<nDataNumber; iloop++)
			 {
				szFormat[0]=0;

				pCgps->GetDataFormat(&nFormatType, iloop, ID_NOTUSED);
				pCgps->GetDataFormatDiscription(szFormat, sizeof(szFormat), nFormatType);

				nIndex=SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)szFormat);
				SendMessage(hwnd, LB_SETITEMDATA, nIndex, nFormatType);
			 }


			 hwnd=GetDlgItem(hDlg, IDC_TO_DATA);
			 MakeDragList(hwnd);

			 nDataNumber=pCgps->GetDataFormatNum(ID_USED);
			 for(iloop=0; iloop<nDataNumber; iloop++)
			 {
				szFormat[0]=0;

				pCgps->GetDataFormat(&nFormatType, iloop, ID_USED);
				pCgps->GetDataFormatDiscription(szFormat, sizeof(szFormat), nFormatType);

				nIndex=SendMessage(hwnd, LB_INSERTSTRING, -1, (LPARAM)szFormat);
				SendMessage(hwnd, LB_SETITEMDATA, nIndex, nFormatType);
			 }
			 return TRUE;

		case WM_COMMAND:
			 pCod=(CCoordinator *)GetWindowLong(hDlg, GWL_USERDATA);

			 switch(LOWORD(wParam))
			 {
				 case IDC_FROM_DATA:
			     case IDC_TO_DATA:
					  if(LBN_DBLCLK != HIWORD(wParam))
						  break;

				 case IDC_SELECT:
				 case IDC_DELETE:
					  nIndex=(IDC_SELECT==LOWORD(wParam) || IDC_FROM_DATA==LOWORD(wParam) )? IDC_FROM_DATA : IDC_TO_DATA;
					  hwnd=GetDlgItem(hDlg, nIndex);
					  
					  nSrcIndex=SendMessage(hwnd, LB_GETCURSEL, 0, 0);
					  if(LB_ERR == nSrcIndex)
						  break;
					  
					  dwData=SendMessage(hwnd, LB_GETITEMDATA, nSrcIndex, 0);
					  if(dwData==ID_FORMAT_NAME)
						  break;

					  SendMessage(hwnd, LB_GETTEXT, nSrcIndex, (LPARAM)szFormat);
					  SendMessage(hwnd, LB_DELETESTRING, nSrcIndex, 0);
					  SendMessage(hwnd, LB_SETCURSEL, (nSrcIndex==0)? nSrcIndex : nSrcIndex-1, 0); 
					  
					  nIndex=IDC_TO_DATA+IDC_FROM_DATA-nIndex ;
					  hwnd=GetDlgItem(hDlg, nIndex);

					  nDestIndex=SendMessage(hwnd, LB_GETCURSEL, 0, 0);
					  if(LB_ERR == nDestIndex)
						  nDestIndex=-1;

					  nDestIndex=SendMessage(hwnd, LB_INSERTSTRING, nDestIndex, (LPARAM)szFormat);
					  SendMessage(hwnd, LB_SETITEMDATA, nDestIndex, dwData);
					  break;


				 case IDOK:
					  hwnd=GetDlgItem(hDlg, IDC_TO_DATA);

					  nDataNumber=SendMessage(hwnd, LB_GETCOUNT, 0, 0);
					  for(iloop=0; iloop<nDataNumber; iloop++)
					  {
						  dwData=SendMessage(hwnd, LB_GETITEMDATA, iloop, 0);
						  nFormatAll[iloop]=dwData;
					  }

					  for(iloop=0; iloop<nReceiverNumber; iloop++)
					  {
			  			  pCgps=pCod->GetGPSreceiver(iloop);
						  pCgps->SetDataFormat(nFormatAll, nDataNumber);
					  }

				 case IDCANCEL:
					  EndDialog(hDlg, TRUE);
					  break;
			 }
			 return FALSE;
	}
    return FALSE;
}
