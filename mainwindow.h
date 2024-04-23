#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void modifyFile(const QDir* dir,  const  QStringList* fileList);
private slots:

    void on_radioButton_operandAnd_clicked();

    void on_radioButton_operandOr_clicked();

    void on_radioButton_operandXor_clicked();

    void on_radioButton_operandNot_clicked();

    void on_radioButton_operandlShift_clicked();

    void on_radioButton_operandRshift_clicked();

    void on_pushButton_masksAdd_clicked();


    void on_pushButton_selectDirectory_clicked();

    void on_lineEdit_2_editingFinished();

    void on_pushButton_workspaceStart_clicked();

    void on_radioButton_iterateTimer_clicked();

    void on_radioButton_iterateOnce_clicked();





    void on_spinBox_valueChanged(int arg1);

    void on_spinBox_2_valueChanged(int arg1);


    void on_lineEdit_textChanged(const QString &arg1);

    void on_pushButton_bitMaskButton_clicked();

    void on_lineEdit_3_textChanged(const QString &arg1);

    void on_radioButton_collisionFilename_clicked();

    void on_radioButton_collisionModify_clicked();

    void on_spinBox_3_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QTimer *myTimer;
    QDir *directory;
    QStringList *files;
    void innerModifyFile(const QDir* directory, const QString& fileName);
    void ANDbytes(char* arr, int size);
    void ORbytes(char* arr, int size);
    void XORbytes(char* arr, int size);
    void NOTbytes(char* arr, int size);
    void leftShift(char* byteArray, int size, int bitsToShift);
    void rightShift(char* byteArray, int size, int bitsToShift);
    bool isShiftOperand = 0;
    bool isTimered = 0;
    int operationType = 0;
    QVector<char> byteOperand;
    int byteCount = 0;
    int shiftCount = 1;
    int innerByteCount = 0;
    std::chrono::milliseconds msCount = std::chrono::milliseconds(0);
    std::chrono::milliseconds msSetTime = std::chrono::milliseconds(1000);
    bool isRunning = 0;
    bool isMaskPresent = 0;
    bool collisionType = 0;

};
#endif // MAINWINDOW_H
