#include"CDeskView.h"
void CDeskView::keyPressEvent(QKeyEvent *event){

	if (event->key() == Qt::Key_Escape){
		emit exit();
	}
}