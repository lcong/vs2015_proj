1 ��mysql.txt�еĽű�����SQL Server �Ĳ�ѯ�������У� ִ�У��������ݿ�ASC

2 ִ��ASC.reg �����ݿ����д��ע�����  λ��HKEY_CURRENT_USER\Software\iXRayASC\Option\DBConnection��

3 �����Ǽٶ���¼���ݿ���û���Ϊ��sa  ����Ϊ��WZY�������ٶ��Ĳ�һ�����޸�ע���HKEY_CURRENT_USER\Software\iXRayASC\Option\DBConnection�µ�LogNm �� LogPWD����һ��ʱ�����޸ĳ���ASC(my)�е����ݿ������ַ������������£�BOOL CASCApp::SetGlobleConnect()
{
	CString connectStr = "driver={SQL Server};Server=127.0.0.1;DATABASE=ASC;UID=sa;PWD=WZY";
������
}
�޸����е�UID ��PWD

4 ����¼���ú�openssh���񣬲�������Կ��֤

5 ������˳���û���ṩ��Ҫģ��������У���� \data\log Ŀ¼�µ����ݿ��������ˣ��ֹ��ϴ����ʼ��������Ķ�Ӧ��logĿ¼�¡�Ϊ�˲��Է���,Ҳ�ɰ�\data\log Ŀ¼�µ�����ֱ�ӿ���\home\username\logĿ¼�½��в���.
