// Sort.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#include <time.h>
#include <sys/timeb.h>




#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	SortMulti(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


///////////////////////////////////////////////////////////////////////////
//����Ľṹ�����ڴ�����������ݸ������������߳�
struct ThreadPrama
{
	long* data;
	int iLength;
	HWND hDlg;
};

struct _timeb timetr1;
struct _timeb timetr2;

ThreadPrama* StructData1 ;
ThreadPrama* StructData2 ;
ThreadPrama* StructData3 ;
ThreadPrama* StructData4 ;
long *data;
int iDataLen;
UINT m_nTimer;
time_t sec1,sec2;


HANDLE hThread[4];

void OnButtonSort(HWND hDlg) ;
int getData(HWND hDlg);
void PrintResult(HWND hDlg ,long* Array, int iLength, int seqList);
void OnButtonClear(HWND hDlg );





//������
DWORD WINAPI BubbleSort(void* theArray);  //ð������
DWORD WINAPI SelectSort(void* theArray);  //ѡ������
DWORD WINAPI HeapSort(void* theArray);    //������
DWORD WINAPI InsertSort(void* theArray);  //��������
/*�����ĸ��������������������Ϊһ���̺߳����ı�Ҫ�����ſ���ʹ��CreateThread
����һ���̡߳�#define PASCAL __stdcall  #define WINAPI FAR PASCAL
��1�����÷���������__stdcall������������ѹջ˳�����ҵ��󣬶����ɺ���������
ջ�Ļָ�, C��C++Ĭ����__cdecl, ����Ҫ��ʽ������__stdcall
��2������ֵ������unsigned long
��3������������һ��32λֵ����һ��ָ��ֵ��long����
 (4) ������������Ա��������������Ϊstatic��������CreateThreadʱ����ָ������
 ���д��������(��������CThreadTest�ĳ�Ա������)��
 static unsigned long _stdcall MyThreadFun(void* pParam);
 handleRet = CreateThread(NULL, 0, &CThreadTestDlg::MyThreadFun, NULL, 0, &ThreadID);
 ֮����Ҫ����Ϊstatic�����ڣ��ú�������Ҫ�����ڶ���ʵ����ʹ�ã���ʹû������ʵ��Ҳ����
 ʹ�á�
*/

int QuickSort(long* Array, int iLow, int iHigh);     //��������

//////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SORT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_SORT);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_SORT);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_SORT;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_CREATE:
			 DialogBox(hInst, (LPCTSTR)IDD_SORT_DIALOG, hWnd, (DLGPROC)SortMulti);
			 break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				case IDM_SORT:
				   DialogBox(hInst, (LPCTSTR)IDD_SORT_DIALOG, hWnd, (DLGPROC)SortMulti);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			RECT rt;
			GetClientRect(hWnd, &rt);
			DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


// Mesage handler for sort box.
LRESULT CALLBACK SortMulti(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hBubbleList = GetDlgItem(hDlg,IDC_LIST_BUBBLE);
	HWND hHeadList = GetDlgItem(hDlg,IDC_LIST_HEAP);
	HWND hInsertList = GetDlgItem(hDlg,IDC_LIST_INSERT);
	HWND hQuickList = GetDlgItem(hDlg,IDC_LIST_QUICK);
	HWND hSelectList = GetDlgItem(hDlg,IDC_LIST_SELECT);

	DWORD dw = 0;


	switch (message)
	{
	case WM_INITDIALOG:
		CheckRadioButton(hDlg,IDC_RADIO_MULTI,IDC_RADIO_SINGLE,IDC_RADIO_SINGLE);
		ListView_SetExtendedListViewStyle(hBubbleList,LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
		ListView_DeleteAllItems( hBubbleList);

		LVCOLUMN column;
		column.mask = LVCF_TEXT;
		column.pszText = "����";
		ListView_InsertColumn(hBubbleList,0,&column);
		column.pszText = "����";
		ListView_InsertColumn(hBubbleList,1,&column);		
		ListView_SetColumnWidth( hBubbleList, 0, 50);
		ListView_SetColumnWidth( hBubbleList, 1, 50);

		ListView_SetExtendedListViewStyle(hHeadList,LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
		ListView_DeleteAllItems( hHeadList);
		column.pszText = "����";
		ListView_InsertColumn(hHeadList,0,&column);
		column.pszText = "����";
		ListView_InsertColumn(hHeadList,1,&column);		
		ListView_SetColumnWidth( hHeadList, 0, 50);
		ListView_SetColumnWidth( hHeadList, 1, 50);

		ListView_SetExtendedListViewStyle(hInsertList,LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
		ListView_DeleteAllItems( hInsertList);
		column.pszText = "����";
		ListView_InsertColumn(hInsertList,0,&column);
		column.pszText = "����";
		ListView_InsertColumn(hInsertList,1,&column);		
		ListView_SetColumnWidth( hInsertList, 0, 50);
		ListView_SetColumnWidth( hInsertList, 1, 50);

		ListView_SetExtendedListViewStyle(hQuickList,LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
		ListView_DeleteAllItems( hQuickList);
		column.pszText = "����";
		ListView_InsertColumn(hQuickList,0,&column);
		column.pszText = "����";
		ListView_InsertColumn(hQuickList,1,&column);		
		ListView_SetColumnWidth( hQuickList, 0, 50);
		ListView_SetColumnWidth( hQuickList, 1, 50);

		ListView_SetExtendedListViewStyle(hSelectList,LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
		ListView_DeleteAllItems( hSelectList);
		column.pszText = "����";
		ListView_InsertColumn(hSelectList,0,&column);
		column.pszText = "����";
		ListView_InsertColumn(hSelectList,1,&column);		
		ListView_SetColumnWidth( hSelectList, 0, 50);
		ListView_SetColumnWidth( hSelectList, 1, 50);		
		return TRUE;
	case WM_TIMER:
		dw = WaitForMultipleObjects(4,hThread,TRUE, 1);
		if (dw != WAIT_TIMEOUT) {
			CloseHandle(hThread[0]);
			CloseHandle(hThread[2]);
			CloseHandle(hThread[3]);  
			CloseHandle(hThread[4]);

			delete[] StructData1->data;
			delete StructData1;
			delete[] StructData2->data;
			delete StructData2;
			delete[] StructData3->data;
			delete StructData3;
			delete[] StructData4->data;
			delete StructData4;
			
			_ftime( &timetr2);		
			time( &sec2);
			long milliseconds=(sec2-sec1)*1000+(timetr2.millitm-timetr1.millitm);
			
			CString str;
			str.Format("%d",milliseconds);
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_TIME),str);
			KillTimer(hDlg,m_nTimer);
 
			TRACE("---------in-------------TIMER---\n");
		}

		
		return TRUE;
		
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
		
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		if (LOWORD(wParam) == IDC_BUTTON_SORT) {
			OnButtonSort(hDlg); 
			return TRUE;
		}
		if (LOWORD(wParam) == IDC_BUTTON_CLEAR) {
			OnButtonClear(hDlg); 
			return TRUE;
		}		
		break;
	}
    return FALSE;
}



void OnButtonSort(HWND hDlg) 
{
	// TODO: Add your control notification handler code here
//	long* data1;// ,*data,*data2,*data3,*data4,*data5
	getData(hDlg);

	time( &sec1);
	_ftime( &timetr1);

	//Ϊ�˶Ը������̷ֱ߳��ԭʼ���ݽ�������ͱ���������
	//�ֱ�����ڴ��data��������ݽ��и���
	long *data1, *data2, *data3, *data4;

	StructData1 = new ThreadPrama;
	StructData2 = new ThreadPrama;
	StructData3 = new ThreadPrama;
	StructData4 = new ThreadPrama;
	data1 = new long[iDataLen];
	
	memcpy(data1, data, iDataLen << 2); //��data�е����ݸ��Ƶ�data1��
	//�ڴ渴�� memcpy(Ŀ���ڴ�ָ��, Դ�ڴ�ָ, �븴���ֽ���), ��Ϊlong�ĳ���
	//Ϊ4�ֽ�,���Ը��Ƶ��ֽ���ΪiDataLen << 2, ������iDataLen*4
	StructData1->data = data1;
	StructData1->iLength = iDataLen;
	StructData1->hDlg = hDlg;

	data2 = new long[iDataLen];
	memcpy(data2, data, iDataLen << 2);
	StructData2->data = data2;
	StructData2->iLength = iDataLen;
	StructData2->hDlg = hDlg;

	data3 = new long[iDataLen];
	memcpy(data3, data, iDataLen << 2);
	StructData3->data = data3;
	StructData3->iLength = iDataLen;
	StructData3->hDlg = hDlg;

	data4 = new long[iDataLen];
	memcpy(data4, data, iDataLen << 2);
	StructData4->data = data4;
	StructData4->iLength = iDataLen;
	StructData4->hDlg = hDlg;



	unsigned long TID1, TID2, TID3, TID4 ; 



	if (IsDlgButtonChecked( hDlg, IDC_RADIO_MULTI) == BST_CHECKED) {
		m_nTimer=SetTimer(hDlg,1, 100, 0);	
		//�ֱ����������߳�
		hThread[0] = CreateThread(NULL, 0, &BubbleSort, StructData1, NULL, &TID1);
		hThread[1] = CreateThread(NULL, 0, &SelectSort, StructData2, NULL, &TID2);
		hThread[2] = CreateThread(NULL, 0, &HeapSort, StructData3, NULL, &TID3);
		hThread[3] = CreateThread(NULL, 0, &InsertSort, StructData4, NULL, &TID4);
		
	
	}	
	else{
		
		BubbleSort(StructData1);
		SelectSort(StructData2);
		HeapSort(StructData3);
		InsertSort(StructData4);
		
		delete[] StructData1->data;
		delete StructData1;
		delete[] StructData2->data;
		delete StructData2;
		delete[] StructData3->data;
		delete StructData3;
		delete[] StructData4->data;
		delete StructData4;
		
		_ftime( &timetr2);		
		time( &sec2);
		long milliseconds=(sec2-sec1)*1000+(timetr2.millitm-timetr1.millitm);
		
		CString str;
		str.Format("%d",milliseconds);
		SetWindowText(GetDlgItem(hDlg,IDC_EDIT_TIME),str);
		
	}
	

	delete[] data;


}

/*
ð������˼��(���򣬽���ͬ��������㷨һ����������):
��ͷ��β�����ݽ��������ȽϽ��н���,С�ķ�ǰ��ķź�
����һ������������Ԫ�ؾͻᱻ���������Ȼ����һ��
ѭ���Ͳ��ö����һ��Ԫ�ؽ��бȽϽ����ˣ�������ÿһ��
�ȽϽ����Ĵ���������һ��ѭ���Ĵ�����һ������n֮��
���ݾͱ������������
*/
DWORD WINAPI BubbleSort(void* theArray)
{
	long* arrayBubble = ((ThreadPrama*)theArray)->data;
	int iLengthBubble = ((ThreadPrama*)theArray)->iLength;
	HWND hDlg = ((ThreadPrama*)theArray)->hDlg;

	int i, j=0;
	long swap;


	for (i = iLengthBubble-1;  i > 0; i--)
	{
		for(j = 0; j < i; j++)
		{
			if(arrayBubble[j] > arrayBubble[j+1])  //ǰ�Ⱥ�󣬽���
			{
				swap = arrayBubble[j];
				arrayBubble[j] = arrayBubble[j+1];
				arrayBubble[j+1] = swap;
			}
			for(int k=0;k<100000;k++);
		 	Sleep(1);
		}
	}

	PrintResult(hDlg,arrayBubble,iLengthBubble, 0); 

return 0;	
}


DWORD WINAPI HeapSort(void* theArray)
{
	long* Array = ((ThreadPrama*)theArray)->data;
	int iLength = ((ThreadPrama*)theArray)->iLength;
	HWND hDlg = ((ThreadPrama*)theArray)->hDlg;

	int i, j, p;
	long swap;

	for(i=0; i<iLength-1; i++)
	{
		for(j = iLength - 1; j>i; j--) //���������ȥ�Ƚ��ֽڵ�͸��ڵ�
		{
			p = (j - i - 1)/2 + i;   //���㸸�ڵ������±�
			//ע�⵽���ڵ������������±겻�ǵ�ͬ�ģ���Ϊ���ѵ�Ԫ�ظ�������ݼ�

			if(Array[j] < Array[p])  //������ڵ���ֵ���򽻻����ڵ���ֽڵ�
			{
				swap = Array[j];
				Array[j] = Array[p];
				Array[p] = swap;
			}
		
			Sleep(1);
		}
	}
	
	PrintResult(hDlg,Array,iLength, 1); 
	return 0;
}

DWORD WINAPI InsertSort(void* theArray)
{
	long* Array = ((ThreadPrama*)theArray)->data;
	int iLength = ((ThreadPrama*)theArray)->iLength;
	HWND hDlg = ((ThreadPrama*)theArray)->hDlg;

	int i=1, j=0;
	long temp;
	for(i=1; i<iLength; i++)
	{
		temp = Array[i];   //ȡ�����к����������ݵĵ�һ��Ԫ��ֵ
		for(j=i; j>0; j--) //��ǰ�����������������бȽ��ҳ����ʵĲ���λ��
		{

			Sleep(1);

			if(Array[j - 1] > temp)  //�����Ԫ�رȲ���ֵ�������
				Array[j] = Array[j - 1];
			else //�����Ԫ�رȲ���ֵС����ô��λ�õĺ�һλ���ǲ���Ԫ�ص�λ��
				break;   
		}
		Array[j] = temp;
	}


	PrintResult(hDlg,Array,iLength, 2); 

	return 0;
}

/*
��������˼�룺
���������Ƿ���˼���һ��Ӧ�ã�����ѡȡһ��֧�㣬Ȼ���С��֧���Ԫ�ؽ���
��֧���ǰ�ߣ��Ѵ���֧���Ԫ�ؽ�����֧����ұߡ�Ȼ���ٶ�֧����߲��ֺ���
�߲��ֽ���ͬ���Ĵ����������ɴ�֮�����ݾͻ�������

�����ʵ��ʹ���˵ݹ�
���������α꣺iLow��iHigh��iLowָ�����еĵ�һ��Ԫ�أ�iHighָ�����һ��

��ѡ��һ��Ԫ����Ϊ֧�㣬��������ֵ������һ�������������ô��һ��λ�þ�
��Ϊ�ղ����Է���������Ԫ�ء� ������iHighָ���Ԫ�ؿ�ʼ��ǰ�ƶ��α�iHigh
���ұ�֧��С��Ԫ�أ�����ҵ�����������õ������˵�λ�ã������ǵ�һ��λ�ã�

Ȼ��iHigh�α�ֹͣ�ƶ�����ʱiHighָ���λ�ñ����ã�Ȼ���ƶ�iLow�α�Ѱ�ұ�
֧����Ԫ�ط��õ�iHighָ��Ŀ��õ�λ�ã��������ֱ��iLow��iHigh��ȡ�
���ʹ�õݹ�����������ֽ���ͬ������
*/

int QuickSort(long* Array, int iLow, int iHigh)
{
	if(iLow >= iHigh) return 1; //�ݹ��������

	long pivot = Array[iLow];

	int iLowSaved = iLow, iHighSaved = iHigh;  //��δ�ı��iLow��iHighֵ��������

	while (iLow < iHigh)
	{

		for(int k=0;k<100000;k++);
		Sleep(5);

		while (Array[iHigh] >= pivot && iHigh > iLow) //Ѱ�ұ�֧����Ԫ��
			iHigh -- ;
		Array[iLow] = Array[iHigh]; //���ҵ���Ԫ�ط��õ����õ�λ��

		while (Array[iLow] < pivot && iLow < iHigh) //Ѱ�ұ�֧��С��Ԫ��
			iLow ++ ;
		Array[iHigh] = Array[iLow]; //���ҵ���Ԫ�ط��õ����õ�λ��
	}
	Array[iLow] = pivot; //��֧��ֵ���õ�֧��λ�ã���ʱ֧��λ���ǿ��õ�

	//�����Ҳ��ֱַ���еݹ鴦��
	QuickSort(Array, iLowSaved, iHigh-1);
	QuickSort(Array, iLow+1, iHighSaved);

	return 0;
}



//ÿһ���̶߳�ʹ����������������

void PrintResult(HWND hDlg ,long* Array, int iLength, int seqList)
{	
	HWND hBubbleList = GetDlgItem(hDlg,IDC_LIST_BUBBLE);
	HWND hHeadList = GetDlgItem(hDlg,IDC_LIST_HEAP);
	HWND hInsertList = GetDlgItem(hDlg,IDC_LIST_INSERT);
	HWND hQuickList = GetDlgItem(hDlg,IDC_LIST_QUICK);
	HWND hSelectList = GetDlgItem(hDlg,IDC_LIST_SELECT);
	
	int i;
	CString strSeq,strData;
	
	
	LVITEM item;
	item.iSubItem = 0;
	item.mask = LVIF_TEXT;
	item.pszText = "";
	
	switch(seqList) {
	case 0 :
		for (i=0; i<iLength; i++)
		{
			strSeq.Format("%d",i);
			strData.Format("%d",Array[i]);
			item.iItem = i;
			ListView_InsertItem(hBubbleList, &item);
			
			ListView_SetItemText(hBubbleList, i, 0,strSeq.GetBuffer(10) );// 
			strSeq.ReleaseBuffer();
			ListView_SetItemText(hBubbleList, i, 1, strData.GetBuffer(10));
			strData.ReleaseBuffer();	
		}
		break;
	case 1:
		for (i=0; i<iLength; i++)
		{
			strSeq.Format("%d",i);
			strData.Format("%d",Array[i]);
			item.iItem = i;
			ListView_InsertItem(hHeadList, &item);
			
			ListView_SetItemText(hHeadList, i, 0,strSeq.GetBuffer(10) );// 
			strSeq.ReleaseBuffer();
			ListView_SetItemText(hHeadList, i, 1, strData.GetBuffer(10));
			strData.ReleaseBuffer();	
		}
		break;
	case 2:
		for (i=0; i<iLength; i++)
		{
			strSeq.Format("%d",i);
			strData.Format("%d",Array[i]);
			item.iItem = i;
			ListView_InsertItem(hInsertList, &item);
			
			ListView_SetItemText(hInsertList, i, 0,strSeq.GetBuffer(10) );// 
			strSeq.ReleaseBuffer();
			ListView_SetItemText(hInsertList, i, 1, strData.GetBuffer(10));
			strData.ReleaseBuffer();
			
		}
		break;
	case 3:
		for (i=0; i<iLength; i++)
		{
			strSeq.Format("%d",i);
			strData.Format("%d",Array[i]);
			item.iItem = i;
			ListView_InsertItem(hQuickList, &item);
			
			ListView_SetItemText(hQuickList, i, 0,strSeq.GetBuffer(10) );// 
			strSeq.ReleaseBuffer();
			ListView_SetItemText(hQuickList, i, 1, strData.GetBuffer(10));
			strData.ReleaseBuffer();
		}
		break;
	default:
		for (i=0; i<iLength; i++)
		{
			strSeq.Format("%d",i);
			strData.Format("%d",Array[i]);
			item.iItem = i;
			ListView_InsertItem(hSelectList, &item);
			
			ListView_SetItemText(hSelectList, i, 0,strSeq.GetBuffer(10) );// 
			strSeq.ReleaseBuffer();
			ListView_SetItemText(hSelectList, i, 1, strData.GetBuffer(10));
			strData.ReleaseBuffer();
		}
		
	}

}







/*
ѡ������˼��:
ÿһ�ζ���������������ҳ���С��Ԫ�أ�Ȼ���ǰ���Ѿ������Ԫ������
�ĺ�һ��Ԫ�ؽ��н�������������Դ���оͻ�ֳ������֣�ǰ��һ�������Ѿ�
�ź�����������У�����һ����������ģ�����ѡ����С��Ԫ�ء�
ѭ��N��֮��ǰ����������мӳ�����Դ����һ��������������򲿷ֳ��ȱ�
Ϊ0�����������ˡ�
*/
DWORD WINAPI SelectSort(void* theArray)
{
	long* Array = ((ThreadPrama*)theArray)->data;
	int iLength = ((ThreadPrama*)theArray)->iLength;
	HWND hDlg = ((ThreadPrama*)theArray)->hDlg;

	long lMin, lSwap;
	int i, j, iMinPos;

	for(i=0; i < iLength-1; i++)
	{
		lMin = Array[i];
		iMinPos = i;

		for(j=i + 1; j <= iLength-1; j++) //�������Ԫ�����ҳ���С��Ԫ��
		{
			if(Array[j] < lMin)
			{
				iMinPos = j;
				lMin = Array[j];
			}

	

		}
		//��ѡ����Ԫ�ؽ���ƴ�ӵ��������е����
		lSwap = Array[i];
		Array[i] = Array[iMinPos];
		Array[iMinPos] = lSwap;


	}
	PrintResult(hDlg,Array,iLength, 4);


	return 0;
}

//******************************************************************************

void OnButtonClear(HWND hDlg )
{

	
	HWND hBubbleList = GetDlgItem(hDlg,IDC_LIST_BUBBLE);
	HWND hHeadList = GetDlgItem(hDlg,IDC_LIST_HEAP);
	HWND hInsertList = GetDlgItem(hDlg,IDC_LIST_INSERT);
	HWND hQuickList = GetDlgItem(hDlg,IDC_LIST_QUICK);
	HWND hSelectList = GetDlgItem(hDlg,IDC_LIST_SELECT);

	ListView_DeleteAllItems( hBubbleList);
	ListView_DeleteAllItems( hHeadList);
	ListView_DeleteAllItems( hInsertList);
	ListView_DeleteAllItems( hQuickList);
	ListView_DeleteAllItems( hSelectList);

	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_TIME),"");
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_DATA),"");
	
}


int getData(HWND hDlg)
{
	iDataLen = 0;
	CString m_strData = "";
	LPTSTR p = m_strData.GetBuffer( 500 );
	GetWindowText(GetDlgItem(hDlg,IDC_EDIT_DATA),p,300);
	m_strData.ReleaseBuffer();

	int i=0,n=0,iTotal;
	CString strTemp,str;

	TRACE("%s---------------------\n",m_strData);
	while (n!=-1){
		n = m_strData.Find(',', i);
		i = n+1;
		if (n!=-1) {
			iDataLen++;
		}
	}
	
	iDataLen++;
	TRACE("----------iDataLen------------%d--------------------------------------------\n",iDataLen);
	strTemp = m_strData;
	data = new long[iDataLen];
	iTotal = m_strData.GetLength();
	for(i=0;i<iDataLen-1;i++)
	{
		n=strTemp.Find(',');
		str=strTemp.Left(n);
		data[i] = atol(str);
		iTotal = iTotal-n-1;
		strTemp=strTemp.Right(iTotal);
	}
	data[i]=atol(strTemp);
	
	for(i=0;i<iDataLen;i++)
	{
		TRACE("-----------iDataLen-1-------------%d---\n",data[i]);
	}	
	return iDataLen;
}
