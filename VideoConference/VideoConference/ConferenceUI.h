#ifndef ConferenceUI_H
#define ConferenceUI_H
#include"CMediaPlayer.h"
#include"CVideoSurface.h"
#include"qlayout.h"
#include"qtimer.h"
#include"MyWindow.h"
#include"CSockChecker.h"
#include"../LibVideoConference/CNetPainter.h"
#include"../LibVideoConference/FDialog.h"
#include"../LibVideoConference/CFileExplorer.h"
#include"../LibVideoConference/CDeskView.h"
#include"../LibVideoConference/CSelectChannel.h"
#include<../LibVideoConference/SQLService.h>
class ConferenceUI :public QWidget
{
	Q_OBJECT
public:
	ConferenceUI(int uid);
	void addplayer(CStreamPlayer*widget, bool isScreenShare); //CVideoSurface CStreamPlayer

	QHBoxLayout *layout;
	QHBoxLayout *toplayout;
	QHBoxLayout *desklayout;
	QHBoxLayout *botlayout;
	QHBoxLayout *Llayout;
	QHBoxLayout *melayout;
	QHBoxLayout *Rlayout;
	QVBoxLayout *mainlayout;

	MyWindow* chat;
	QPushButton *closeBtn;
	QPushButton *fullBtn;
	FDialog* fd;
	CFileExplorer* cfe;
	//////////////////////////////////////////////
	void addMember(char* vip,  char* aip, char* sip, int id);
	CStreamPlayer* players[100];
	CStreamPlayer* deskplayers[100];
	int playerCnt;

	CSockChecker *sock;
	int myid;

	CNetPainter *painter;
	//////////////////////////////
	void setMid(int);
	bool isSameMeeting(int mid);
	void setUserDataList(map<int, CUserData>);
	void closeAll();
	void addByIdlist();
	void setMyid(int);
	void closeEvent(QCloseEvent * event);
	/*for full screen desk view*/
	CDeskView qw;
	QHBoxLayout*fl;
	int nowfullshow = 0;
	/*for select channel*/
	CSelectChannel *csc;
	
	QString namelist[100];
	QPushButton *selectChannelBtn;
	
	/////////////////////////
	signals:
	void exitConference(int mid);     //離開會議訊號
	public slots:
	void closeBtn_clicked();
	void selectChannelBtn_clicked();
	void fullBtn_clicked();
	void newCon();
	void showBychecklist();
private :
	CMeetingData md;
	int	mid;
	void setMemberlist(int *);
};
#endif // 