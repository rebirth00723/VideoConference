#ifndef QIPLINEEDIT_H
#define QIPLINEEDIT_H

#include <QLineEdit>
#include <QEvent>

class QIPLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	QIPLineEdit(QWidget *parent = 0);
	~QIPLineEdit();

	void setText(const QString &strIP);
	QString text() const;
protected:
	void paintEvent(QPaintEvent *event);
	bool eventFilter(QObject *obj, QEvent *ev);

	int getIndex(QLineEdit *pEdit);
	bool isTextValid(const QString &strIP);
private:
	QLineEdit *m_lineEidt[4];
};

#endif // QIPLINEEDIT_H