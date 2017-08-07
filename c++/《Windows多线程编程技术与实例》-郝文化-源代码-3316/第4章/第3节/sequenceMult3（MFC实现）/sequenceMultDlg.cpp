// sequenceMultDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sequenceMult.h"
#include "sequenceMultDlg.h"


#include "stdlib.h"
#include "memory.h"

#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSequenceMultDlg dialog

CSequenceMultDlg::CSequenceMultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSequenceMultDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSequenceMultDlg)
	m_strData = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	hasPrintBubble=FALSE;
	hasPrintHeap = FALSE;
	hasPrintSelect=FALSE;
	hasPrintInsert=FALSE;
	iDataLen = 0;

}

void CSequenceMultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSequenceMultDlg)
	DDX_Control(pDX, IDC_LIST_SELECT, m_selectList);
	DDX_Control(pDX, IDC_LIST_QUICK, m_quickList);
	DDX_Control(pDX, IDC_LIST_INSERT, m_insertList);
	DDX_Control(pDX, IDC_LIST_HEAP, m_headList);
	DDX_Control(pDX, IDC_LIST_BUBBLE, m_bubbleList);
	DDX_Text(pDX, IDC_EDIT_DATA, m_strData);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSequenceMultDlg, CDialog)
	//{{AFX_MSG_MAP(CSequenceMultDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SORT, OnButtonSort)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSequenceMultDlg message handlers


///////////////////////////////////////////////////////////////////////////



HANDLE evtTerminate; //�¼��ź�,����Ƿ��������̶߳�ִ����
HANDLE evtPrint;
HANDLE evtHeap;
HANDLE evtBubble;
HANDLE evtInsert;
HANDLE evtQuick;
HANDLE evtSelect;



/*
����ʹ�������ֿ��Ʒ����������ע���������֣�ʹ������һ�֡�
ע���޸�ʱҪ�����޸��ٽ���PrintResult�����Ӧ�������
*/
//HANDLE evtPrint;   //�¼��ź�,����¼��Ƿ��ѷ���
//CRITICAL_SECTION csPrint;   //�ٽ���
//HANDLE mtxPrint; //�����ź�,�����źű����Ѿ����߳̽����ٽ�����ӵ�д��ź�

static long ThreadCompleted = 0;  




/*��������ĸ����߳���������̵߳ĸ���,
��һ�����߳����ʱ�Ͷ�ThreadCompleted���м�һ����, 
Ҫʹ��InterlockedIncrement(long* lpAddend)��
InterlockedDecrement(long* lpAddend)���мӼ�����
*/

//����Ľṹ�����ڴ�����������ݸ������������߳�
struct MySafeArray
{
	long* data;
	int iLength;
};


//��ӡÿһ���̵߳�������



//������
unsigned long __stdcall BubbleSort(void* theArray);  //ð������
unsigned long __stdcall SelectSort(void* theArray);  //ѡ������
unsigned long __stdcall HeapSort(void* theArray);    //������
unsigned long __stdcall InsertSort(void* theArray);  //��������
/*�����ĸ��������������������Ϊһ���̺߳����ı�Ҫ�����ſ���ʹ��CreateThread
����һ���̡߳�
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




///////////////////////////////////////////////////////////////////////////////////







BOOL CSequenceMultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// TODO: Add extra initialization her
	((CButton*)GetDlgItem(IDC_RADIO_MULTI))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_SINGLE))->SetCheck(1);	

	RECT rect;
	m_bubbleList.ModifyStyle(LVS_TYPEMASK,LVS_REPORT);
	ListView_SetExtendedListViewStyle(m_bubbleList.m_hWnd,LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_bubbleList.DeleteAllItems();
	m_bubbleList.InsertColumn(0,"����");
	m_bubbleList.InsertColumn(1,"����");
	m_bubbleList.GetWindowRect(&rect);
	int nTotalX = rect.right-rect.left;
	m_bubbleList.SetColumnWidth(0,2*nTotalX/5);
	m_bubbleList.SetColumnWidth(1,3*nTotalX/5);

	m_headList.ModifyStyle(LVS_TYPEMASK,LVS_REPORT);
	ListView_SetExtendedListViewStyle(m_headList.m_hWnd,LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_headList.DeleteAllItems();
	m_headList.InsertColumn(0,"����");
	m_headList.InsertColumn(1,"����");
	m_headList.GetWindowRect(&rect);
	nTotalX = rect.right-rect.left;
	m_headList.SetColumnWidth(0,2*nTotalX/5);
	m_headList.SetColumnWidth(1,3*nTotalX/5);

	m_insertList.ModifyStyle(LVS_TYPEMASK,LVS_REPORT);
	ListView_SetExtendedListViewStyle(m_insertList.m_hWnd,LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_insertList.DeleteAllItems();
	m_insertList.InsertColumn(0,"����");
	m_insertList.InsertColumn(1,"����");
	m_insertList.GetWindowRect(&rect);
	nTotalX = rect.right-rect.left;
	m_insertList.SetColumnWidth(0,2*nTotalX/5);
	m_insertList.SetColumnWidth(1,3*nTotalX/5);

	m_quickList.ModifyStyle(LVS_TYPEMASK,LVS_REPORT);
	ListView_SetExtendedListViewStyle(m_quickList.m_hWnd,LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_quickList.DeleteAllItems();
	m_quickList.InsertColumn(0,"����");
	m_quickList.InsertColumn(1,"����");
	m_quickList.GetWindowRect(&rect);
	nTotalX = rect.right-rect.left;
	m_quickList.SetColumnWidth(0,2*nTotalX/5);
	m_quickList.SetColumnWidth(1,3*nTotalX/5);

	m_selectList.ModifyStyle(LVS_TYPEMASK,LVS_REPORT);
	ListView_SetExtendedListViewStyle(m_selectList.m_hWnd,LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_selectList.DeleteAllItems();
	m_selectList.InsertColumn(0,"����");
	m_selectList.InsertColumn(1,"����");
	m_selectList.GetWindowRect(&rect);
	nTotalX = rect.right-rect.left;
	m_selectList.SetColumnWidth(0,2*nTotalX/5);
	m_selectList.SetColumnWidth(1,3*nTotalX/5);



	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSequenceMultDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSequenceMultDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSequenceMultDlg::OnButtonSort() 
{
	// TODO: Add your control notification handler code here
	getData();


	time( &sec1);
	_ftime( &timetr1);




//	long data[] = {907,788,566,556,3434,4556,2234,4354,5656,23445,1,3,2,54,13,112,23,56,67,76,66,55,33,22,11,76,78,89,85,57,200,3003,1235,678,345,678,789,4567,6789,1223,1004,123,34,546,754,34,74,3,56,2,4,6,111,121,145,123,1111,1234,1543,1789,1987};
//	iDataLen = 61;

	//Ϊ�˶Ը������̷ֱ߳��ԭʼ���ݽ�������ͱ���������
	//�ֱ�����ڴ��data��������ݽ��и���

	MySafeArray StructData1, StructData2, StructData3, StructData4;

	data1 = new long[iDataLen];
	
	memcpy(data1, data, iDataLen << 2); //��data�е����ݸ��Ƶ�data1��
	//�ڴ渴�� memcpy(Ŀ���ڴ�ָ��, Դ�ڴ�ָ, �븴���ֽ���), ��Ϊlong�ĳ���
	//Ϊ4�ֽ�,���Ը��Ƶ��ֽ���ΪiDataLen << 2, ������iDataLen*4
	StructData1.data = data1;
	StructData1.iLength = iDataLen;

	data2 = new long[iDataLen];
	memcpy(data2, data, iDataLen << 2);
	StructData2.data = data2;
	StructData2.iLength = iDataLen;

	data3 = new long[iDataLen];
	memcpy(data3, data, iDataLen << 2);
	StructData3.data = data3;
	StructData3.iLength = iDataLen;

	data4 = new long[iDataLen];
	memcpy(data4, data, iDataLen << 2);
	StructData4.data = data4;
	StructData4.iLength = iDataLen;

	data5 = new long[iDataLen];
	memcpy(data5, data, iDataLen << 2);


	unsigned long TID1, TID2, TID3, TID4 ; 


	//���ź������г�ʼ��

	evtTerminate = CreateEvent(NULL, FALSE, FALSE, "Terminate");
	evtPrint = CreateEvent(NULL, FALSE, TRUE, "PrintResult");
	evtHeap = CreateEvent(NULL, FALSE, FALSE, "heap");
	evtBubble = CreateEvent(NULL, FALSE, FALSE, "bubble");
	evtInsert = CreateEvent(NULL, FALSE, FALSE, "insert");
	evtQuick = CreateEvent(NULL, FALSE, FALSE, "Quick");
	evtSelect = CreateEvent(NULL, FALSE, FALSE, "select");
	//mtxPrint = CreateMutex(NULL, FALSE, "PrintMutex");
	//InitializeCriticalSection(&csPrint);

	m_nTimer=SetTimer(1, 100, 0);


	if (((CButton*)GetDlgItem(IDC_RADIO_SINGLE))->GetCheck()==0) {
	
	//�ֱ����������߳�
	CreateThread(NULL, 0, &BubbleSort, &StructData1, NULL, &TID1);
	CreateThread(NULL, 0, &SelectSort, &StructData2, NULL, &TID2);
	CreateThread(NULL, 0, &HeapSort, &StructData3, NULL, &TID3);
	CreateThread(NULL, 0, &InsertSort, &StructData4, NULL, &TID4);
	TRACE("--------------------enter thread----------\n");
		
	}	
	else{
		
	BubbleSort(&StructData1);
	SelectSort(&StructData2);
	HeapSort(&StructData3);
	InsertSort(&StructData4);
	}


	//�����߳���ִ���п��������������������߳���ִ��
// 	QuickSort(data5, 0, iDataLen - 1);
	


	


	delete[] data5;
	delete[] data;

//	CloseHandle(evtPrint);


}

/*
ð������˼��(���򣬽���ͬ��������㷨һ����������):
��ͷ��β�����ݽ��������ȽϽ��н���,С�ķ�ǰ��ķź�
����һ������������Ԫ�ؾͻᱻ���������Ȼ����һ��
ѭ���Ͳ��ö����һ��Ԫ�ؽ��бȽϽ����ˣ�������ÿһ��
�ȽϽ����Ĵ���������һ��ѭ���Ĵ�����һ������N��֮��
���ݾͱ������������
*/
unsigned long __stdcall BubbleSort(void* theArray)
{
	long* arrayBubble = ((MySafeArray*)theArray)->data;
	int iLengthBubble = ((MySafeArray*)theArray)->iLength;

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


	InterlockedIncrement(&ThreadCompleted);      //����ǰʹ�߳��������Ǽ�1
	if(ThreadCompleted == 4) SetEvent(evtTerminate);
	SetEvent(evtBubble); //����Ƿ������̶߳���ִ����
	//����ִ���������ó�������ź���

return 0;	
}


unsigned long __stdcall HeapSort(void* theArray)
{
	long* Array = ((MySafeArray*)theArray)->data;
	int iLength = ((MySafeArray*)theArray)->iLength;

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
	

//	PrintResult(Array, iLength, "Heap Sort"); //�����̨��ӡ������
	InterlockedIncrement(&ThreadCompleted);   //����ǰʹ�߳��������Ǽ�1
	if(ThreadCompleted == 4) SetEvent(evtTerminate); //����Ƿ������̶߳���ִ����
	//����ִ���������ó�������ź���
	SetEvent(evtHeap);
	return 0;
}

unsigned long __stdcall InsertSort(void* theArray)
{
	long* Array = ((MySafeArray*)theArray)->data;
	int iLength = ((MySafeArray*)theArray)->iLength;

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



	InterlockedIncrement(&ThreadCompleted);  //����ǰʹ�߳��������Ǽ�1
	if(ThreadCompleted == 4)  //����Ƿ������̶߳���ִ����
		SetEvent(evtTerminate);//����ִ���������ó�������ź���
	
	SetEvent(evtInsert);
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

//*****************************�ٽ���***************************************

//ÿһ���̶߳�Ҫʹ����������������������ֻ��һ����ʾ������������߳�
//�����Կ���̨��ʹ��Ȩ��
void CSequenceMultDlg::PrintResult(long* Array, int iLength, int seqList)
{

WaitForSingleObject(evtPrint, 0);
	
	int i;
	CString strSeq,strData;
	switch(seqList) {
	case 0 :
		for (i=0; i<iLength; i++)
		{
			strSeq.Format("%d",i);
			strData.Format("%d",Array[i]);
			m_bubbleList.InsertItem(i,"");
			m_bubbleList.SetItemText(i,0,strSeq);
			m_bubbleList.SetItemText(i,1,strData);
			
		}
		break;
	case 1:
		for (i=0; i<iLength; i++)
		{
			strSeq.Format("%d",i);
			strData.Format("%d",Array[i]);
			m_headList.InsertItem(i,"");
			m_headList.SetItemText(i,0,strSeq);
			m_headList.SetItemText(i,1,strData);
			
		}
		break;
	case 2:
		for (i=0; i<iLength; i++)
		{
			strSeq.Format("%d",i);
			strData.Format("%d",Array[i]);
			m_insertList.InsertItem(i,"");
			m_insertList.SetItemText(i,0,strSeq);
			m_insertList.SetItemText(i,1,strData);
			
		}
		break;
	case 3:
		for (i=0; i<iLength; i++)
		{
			strSeq.Format("%d",i);
			strData.Format("%d",Array[i]);
			m_quickList.InsertItem(i,"");
			m_quickList.SetItemText(i,0,strSeq);
			m_quickList.SetItemText(i,1,strData);
			
		}
		break;
	default:
		for (i=0; i<iLength; i++)
		{
			strSeq.Format("%d",i);
			strData.Format("%d",Array[i]);
			m_selectList.InsertItem(i,"");
			m_selectList.SetItemText(i,0,strSeq);
			m_selectList.SetItemText(i,1,strData);
			
		}
		
	}
	SetEvent(evtPrint);
}







/*
ѡ������˼��:
ÿһ�ζ���������������ҳ���С��Ԫ�أ�Ȼ���ǰ���Ѿ������Ԫ������
�ĺ�һ��Ԫ�ؽ��н�������������Դ���оͻ�ֳ������֣�ǰ��һ�������Ѿ�
�ź�����������У�����һ����������ģ�����ѡ����С��Ԫ�ء�
ѭ��N��֮��ǰ����������мӳ�����Դ����һ��������������򲿷ֳ��ȱ�
Ϊ0�����������ˡ�
*/
unsigned long __stdcall SelectSort(void* theArray)
{
	long* Array = ((MySafeArray*)theArray)->data;
	int iLength = ((MySafeArray*)theArray)->iLength;

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

//	PrintResult(Array, iLength, "Select Sort"); //�����̨��ӡ������
	InterlockedIncrement(&ThreadCompleted);  //����ǰʹ�߳��������Ǽ�1
	if(ThreadCompleted == 4) SetEvent(evtTerminate);//����Ƿ������̶߳���ִ����
	//����ִ���������ó�������ź���

	SetEvent(evtSelect);
	return 0;
}

//******************************************************************************

void CSequenceMultDlg::OnButtonClear() 
{
	// TODO: Add your control notification handler code here
	m_bubbleList.DeleteAllItems();
	m_headList.DeleteAllItems();
	m_insertList.DeleteAllItems();
	m_quickList.DeleteAllItems();
	m_selectList.DeleteAllItems();
	GetDlgItem(IDC_EDIT_TIME)->SetWindowText("");
	GetDlgItem(IDC_EDIT_DATA)->SetWindowText("");

	hasPrintBubble = FALSE;
	hasPrintInsert = FALSE;
	hasPrintSelect = FALSE;
	hasPrintHeap = FALSE;

	ThreadCompleted = 0;

	
}

void CSequenceMultDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

		if (WaitForSingleObject(evtHeap, 0)==WAIT_OBJECT_0) {
			if (!hasPrintHeap) {
			PrintResult(data3, iDataLen, 1);  
			hasPrintHeap=TRUE;
			TRACE("----------print2-------------\n");
			delete[] data3;


			}
		}

		if (WaitForSingleObject(evtBubble, 0)==WAIT_OBJECT_0) {
			TRACE("---------------------------WAIT\n");
			if ( !hasPrintBubble ) 
			{
			
			PrintResult(data1, iDataLen, 0);  
			hasPrintBubble = TRUE;
			TRACE("----------print1-------------\n");
			delete[] data1;

			}
		}

		if (WaitForSingleObject(evtSelect, 0)==WAIT_OBJECT_0) {
			if (!hasPrintSelect) {
				PrintResult(data2, iDataLen, 4);  
				hasPrintSelect = TRUE;
	
				delete[] data2;
			}
		}
		if (WaitForSingleObject(evtInsert, 0)==WAIT_OBJECT_0) {
			 if (!hasPrintInsert){
				PrintResult(data4, iDataLen, 2);  
				hasPrintSelect = TRUE;
		
				delete[] data4;
			}
		}
			
			//�ȴ����е����߳̽���
			//���е����߳̽��������̲߳ſ��Խ���
		if (WaitForSingleObject(evtTerminate, 0)==WAIT_OBJECT_0) {
			_ftime( &timetr2);
			
			time( &sec2);
			milliseconds=(sec2-sec1)*1000+(timetr2.millitm-timetr1.millitm);
			
			CString str;
			str.Format("%d",milliseconds);
			GetDlgItem(IDC_EDIT_TIME)->SetWindowText(str);
			KillTimer(m_nTimer);
		}	
			
// 	CDialog::OnTimer(nIDEvent);
}

void CSequenceMultDlg::getData()
{
	UpdateData(TRUE);
iDataLen=0;
	int i=0,n=0,iTotal;
	CString strTemp,str;

	 while (n!=-1){
		n = m_strData.Find(',', i);
		TRACE("=======---------------n------------%d\n",n);
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
			TRACE("------inin-----iDataLen-1-------------%d---\n",data[i]);
		iTotal = iTotal-n-1;
		strTemp=strTemp.Right(iTotal);
	}
	data[i]=atol(strTemp);

		for(i=0;i<iDataLen;i++)
	{
 	TRACE("-----------iDataLen-1-------------%d---\n",data[i]);
		}	

	


}
