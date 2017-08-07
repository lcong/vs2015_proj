// drawLineView.cpp : implementation of the CDrawLineView class
//

#include "stdafx.h"
#include "drawLine.h"

#include "drawLineDoc.h"
#include "drawLineView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// CDrawLineView

IMPLEMENT_DYNCREATE(CDrawLineView, CView)

BEGIN_MESSAGE_MAP(CDrawLineView, CView)
	//{{AFX_MSG_MAP(CDrawLineView)
	ON_COMMAND(ID_DRAW_LINE_THREAD, OnDrawLineThread)
	ON_UPDATE_COMMAND_UI(ID_DRAW_LINE_THREAD, OnUpdateDrawLineThread)
	ON_COMMAND(ID_DRAW_LINE_TIMER, OnDrawLineTimer)
	ON_UPDATE_COMMAND_UI(ID_DRAW_LINE_TIMER, OnUpdateDrawLineTimer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawLineView construction/destruction

CDrawLineView::CDrawLineView()
{
	// TODO: add construction code here
	for(int i=0;i<THREAD_NUM;i++){
		m_pThread[i]=NULL;//ָ���̵߳������ʼ��Ϊ��
	}
}

CDrawLineView::~CDrawLineView()
{
}

BOOL CDrawLineView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDrawLineView drawing

void CDrawLineView::OnDraw(CDC* pDC)
{
	CDrawLineDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CDrawLineView diagnostics

#ifdef _DEBUG
void CDrawLineView::AssertValid() const
{
	CView::AssertValid();
}

void CDrawLineView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDrawLineDoc* CDrawLineView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDrawLineDoc)));
	return (CDrawLineDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDrawLineView message handlers

void CDrawLineView::OnDrawLineThread() 
{
	PARAM *m_pParam[THREAD_NUM]={NULL,NULL,NULL};//����������ں���
	                                             //�Ĳ�������ʼ��Ϊ��
	for(int i=0;i<THREAD_NUM;i++){
		
		m_pThread[i]=NULL;
		m_pParam[i]=new PARAM;
		memset((void*)m_pParam[i],0,sizeof(PARAM));//��Ϊ��ں�����������
												   //���ڴ�ȫ����0ֵ
		//д���߳�ID�ʹ��ھ��
		m_pParam[i]->m_nID=i;
		m_pParam[i]->m_hWnd=GetSafeHwnd();
		//�����߳�
		m_pThread[i]=AfxBeginThread(DrawLine,(LPVOID)(m_pParam[i]));
	}
}

void CDrawLineView::OnUpdateDrawLineThread(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	//���߳�ȫ���������˵�תΪ��ɫ�������ã�
	pCmdUI->Enable(m_pThread[0]==NULL&&
			m_pThread[1]==NULL&&
			m_pThread[2]==NULL);
}

void CDrawLineView::OnDrawLineTimer() 
{
	// TODO: Add your command handler code here
	
}

void CDrawLineView::OnUpdateDrawLineTimer(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

//������ں���
UINT DrawLine(LPVOID pParam)
{
	PARAM *m_pParam=(PARAM*)pParam;
	CBrush m_Brush;
	CWnd m_Wnd;
	CDC *m_Dc;

	//�����߳�ID������Ӧ��ɫ�Ļ�ˢ
	if(m_pParam->m_nID==0)
	{
		m_Brush.CreateSolidBrush(RGB(255,0,0));
	}else if(m_pParam->m_nID==1){
		m_Brush.CreateSolidBrush(RGB(0,255,0));
	}else{
		m_Brush.CreateSolidBrush(RGB(0,0,255));
	}
	
	//���ݴ��ھ�����õ�CDC
	m_Wnd.Attach(m_pParam->m_hWnd);
	m_Dc=m_Wnd.GetDC();
	CBrush *m_pOldBrush=(CBrush*)m_Dc->SelectObject(&m_Brush);
	
	//�ظ������Σ�ÿ��ǰ��һ������
	for(int i=1;i<300;i++){
		m_Dc->Rectangle(10,35+m_pParam->m_nID*50,10+i,55+m_pParam->m_nID*50);
		Sleep(100);//���߳���ͣ100ms����Ϊ�˿������ԵĿ��������߳���ͬʱִ�е�
				   //(�����ҵ����ۿ����������ģ�ʵ�ʲ������)
	}

	m_Dc->SelectObject(m_pOldBrush);
	m_Wnd.Detach();
	return 0;
}//DrawLine