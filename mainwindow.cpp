#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QWidget>
#include <QRadioButton>
#include <QFileDialog>
#include <QButtonGroup>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <bitset>
#include <QTimer>
#include <chrono>
void MainWindow::leftShift(char* byteArray, int size, int bitsToShift) {
    int byteShift = bitsToShift / 8;
    int bitShift = bitsToShift % 8;

    for (int i = byteShift; i < size; ++i) {
        byteArray[i - byteShift] = byteArray[i];
    }

    for (int i = size - byteShift; i < size; ++i) {
        byteArray[i] = 0;
    }

    for (int i = 0; i < size; ++i) {
        byteArray[i] <<= bitShift;
        if (i + 1 < size) {
            byteArray[i] |= byteArray[i + 1] >> (8 - bitShift);
        }
    }
}

void MainWindow::rightShift( char* byteArray, int size, int bitsToShift) {
    int byteShift = bitsToShift / 8;
    int bitShift = bitsToShift % 8;

    for (int i = size - 1 - byteShift; i >= 0; --i) {
        byteArray[i + byteShift] = byteArray[i];
    }

    for (int i = 0; i < byteShift; ++i) {
        byteArray[i] = 0;
    }

    for (int i = size - 1; i >= 0; --i) {
        byteArray[i] >>= bitShift;
        if (i > 0) {
            byteArray[i] |= byteArray[i - 1] << (8 - bitShift);
        }
    }
}




bool isValidFileMask(const QString& mask) {
    // Regular expression pattern for a valid file mask
    QRegularExpression regex("^[\\w\\*\\.\\?\\[\\]\\(\\)]+$");

    // Check if the mask matches the pattern
    return regex.match(mask).hasMatch();
}

QString charToBinary(char byte) {
    // Create a bitset with the byte value
    std::bitset<8> bits(byte);

    // Convert the bitset to a string
    std::string binaryString = bits.to_string();

    // Convert the binary string to a QString
    return QString::fromStdString(binaryString);
}


char convertToByte(const QString& bitmask) {

    char result = 0;
    for (int i = 0; i < 8; ++i) {
        QChar bit = bitmask.at(i);
        if (bit == '1') {
            result |= (1 << (7 - i)); // Set the corresponding bit in result
        }
    }
    return result;
}
void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1.size()!= 8) {
        ui->lineEdit->setProperty("state", "0");
    } else {
        ui->lineEdit->setProperty("state", "1");
    }
    style()->polish(ui->lineEdit);
}




void MainWindow::ANDbytes(char* arr, int size) {
    for(int i = 0; i < size; i++) {
        arr[i] &= byteOperand[i%byteOperand.size()];
    }
}

void MainWindow::ORbytes(char* arr, int size) {
    for(int i = 0; i < size; i++) {
        arr[i] |= byteOperand[i%byteOperand.size()];
    }
}
void MainWindow::XORbytes(char* arr, int size) {
    for(int i = 0; i < size; i++) {
        arr[i] ^= byteOperand[i%byteOperand.size()];
    }
}
void MainWindow::NOTbytes(char* arr, int size) {
    for(int i = 0; i < size; i++) {
        arr[i] = ~arr[i];
    }
}
void MainWindow::modifyFile(const QDir* dir,  const QStringList* fileList) {
    if(ui->listWidget_workspaceListWidget->count()) {
        ui->listWidget_workspaceListWidget->clear();
    }

    for (int i = 0; i < fileList->count(); i++) {
            ui->listWidget_workspaceListWidget->addItem((*fileList)[i]);
            innerModifyFile(dir, (*fileList)[i]);
        }

}
void MainWindow::innerModifyFile(const QDir* directory, const QString& fileName) {
    int bufferSize = ui->spinBox->value() * 128;

    QFile inputFile(directory->absoluteFilePath(fileName));
        if (inputFile.open(QIODevice::ReadOnly)) {
            QFile* outputFile;
            QString baseName = fileName;
            QString extension;
            int dotIndex = fileName.lastIndexOf('.');
            if (dotIndex != -1) {
                baseName = fileName.left(dotIndex);
                extension = fileName.mid(dotIndex);
            }
            baseName+= "_copy";
            if(collisionType) {
                outputFile = new QFile(directory->absoluteFilePath(baseName + extension));
            } else {
                QString newFilename = baseName;
                int counter = 1;

                while (QFile::exists(directory->absoluteFilePath(newFilename + extension))) {
                    newFilename = baseName + QString::number(counter++);
                }
                newFilename += extension;
                outputFile = new QFile(directory->absoluteFilePath(newFilename));
            }

            if (outputFile->open(QIODevice::WriteOnly)) {
                char *buffer = new char[bufferSize];
                qint64 bytesRead;
                while ((bytesRead = inputFile.read(buffer, bufferSize)) > 0) {
                    if(operationType == 0) {
                        this->ANDbytes(buffer, static_cast<int>(bytesRead));
                    } else if (operationType == 1) {
                        this->ORbytes(buffer, static_cast<int>(bytesRead));
                    }else if (operationType == 2) {
                        this->XORbytes(buffer, static_cast<int>(bytesRead));
                    }else if (operationType == 3) {
                        this->NOTbytes(buffer, static_cast<int>(bytesRead));
                    }else if (operationType == 4) {

                        int temp = ui->spinBox->value();
                        int i;
                        for(i = 0; i < bytesRead / temp; i++) {
                            this->leftShift(buffer + (i*temp), temp, ui->spinBox_2->value());
                        }

                        if(bytesRead % temp) {
                            this->leftShift(buffer + (i*temp), bytesRead % temp, ui->spinBox_2->value());
                        }


                    }else if (operationType == 5) {
                        int temp = ui->spinBox->value();
                        int i;
                        for(i = 0; i < bytesRead / temp; i++) {
                            this->rightShift(buffer + (i*temp), temp, ui->spinBox_2->value());
                        }

                        if(bytesRead % temp) {
                            this->rightShift(buffer + (i*temp), bytesRead % temp, ui->spinBox_2->value());
                        }

                    }
                    outputFile->write(buffer, bytesRead);
                }

                // Close the output file
                delete[] buffer;
                outputFile->close();
                delete outputFile;
            } else {
                // Handle error opening the output file
                QMessageBox::critical(nullptr, "Error", "Error opening output file for writing: " + outputFile->errorString() );

            }

            // Close the input file
            inputFile.close();
        } else {
            // Handle error opening the input file
            qDebug() << "Error opening input file for reading: " << inputFile.errorString();
        }
}
void MainWindow::on_pushButton_workspaceStart_clicked()
{
    if(!isRunning && isMaskPresent) {
        if(ui->listWidget_workspaceListWidget->count()) {
            ui->listWidget_workspaceListWidget->clear();
        }

        if(ui->lineEdit->text().size() != 8) {
            ui->lineEdit->setText("00000000");
        }


        if(byteCount != byteOperand.size() && operationType < 4) {
            QMessageBox msgBox;
            msgBox.setText("Not all bytes are assigned to custom bitmask. Do you want to fill remaining bytes with global bitmask?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);

            // Display the message box and wait for the user's choice
            int choice = msgBox.exec();
            if(choice == QMessageBox::Yes) {

                for(int i = byteCount; i < byteOperand.size(); i++) {

                    byteOperand[i] = convertToByte(ui->lineEdit->text());
                }
                if(byteCount == 0) {
                    ui->lineEdit_3->setText(charToBinary(byteOperand[0]));
                }

            } else {
                return;
            }
        }
        if(byteOperand.size() == 1) {
            byteOperand[0] = convertToByte(ui->lineEdit->text());
        }

        directory =  new QDir(ui->lineEdit_2->text());
        QString fileMask = ui->label_maskSettingCurrentMask->text();
        files = new QStringList(directory->entryList(QStringList(fileMask), QDir::Files | QDir::NoDotAndDotDot, QDir::Name));
        if(isTimered) {

            isRunning = 1;

            ui->lineEdit_2->setEnabled(0);
            ui->pushButton_selectDirectory->setEnabled(0);
            ui->checkBox_deleteFiles->setEnabled(0);
            ui->radioButton_collisionFilename->setEnabled(0);
            ui->radioButton_collisionModify->setEnabled(0);
            ui->radioButton_iterateOnce->setEnabled(0);
            ui->radioButton_iterateTimer->setEnabled(0);

            if(isTimered) {
                ui->spinBox_3->setEnabled(0);
            }


            ui->radioButton_operandAnd->setEnabled(0);
            ui->radioButton_operandOr->setEnabled(0);
            ui->radioButton_operandXor->setEnabled(0);
            ui->radioButton_operandNot->setEnabled(0);
            ui->radioButton_operandlShift->setEnabled(0);
            ui->radioButton_operandRshift->setEnabled(0);

            if (operationType != 3) {
                if(operationType != 4 && operationType != 5) {
                    ui->groupBox->setEnabled(0);
                    ui->groupBox_2->setEnabled(0);
                }
                    ui->groupBox_3->setEnabled(0);
                    ui->groupBox_5->setEnabled(0);
            }
            ui->lineEdit_masksAdd->setEnabled(0);
            ui->pushButton_masksAdd->setEnabled(0);



            ui->label_workspaceStatus->setText("Remaining time: " + QString::number(static_cast<int>(msSetTime.count() / 1000)));
            myTimer = new QTimer(this);
            QObject::connect(myTimer, &QTimer::timeout, this, [&]() {
                this->modifyFile(directory, files);
            });
            QObject::connect(myTimer, &QTimer::timeout, this, [&]() {
                msCount += std::chrono::milliseconds(1000);
                ui->label_workspaceStatus->setText("Remaining time: " + QString::number(static_cast<int>((msSetTime-msCount).count()) / 1000));
            });
            myTimer->start(1000);
            QTimer::singleShot(msSetTime, this, [&]() {
                myTimer->stop();
                QObject::disconnect(myTimer);
                ui->label_workspaceStatus->setText("awaits...");

                msCount = std::chrono::milliseconds(0);

                isRunning = 0;

                if(ui->checkBox_deleteFiles->isChecked()) {
                    for (const QString& fileName : *files) {
                        // Construct the absolute file path
                        QString absoluteFilePath = directory->filePath(fileName);

                        // Remove the file
                        if (!QFile::remove(absoluteFilePath)) {
                            // Handle error if removal fails
                            QMessageBox::critical(nullptr, "Error", "Failed to delete file:" + absoluteFilePath);
                        }
                    }
                }

                ui->lineEdit_2->setEnabled(1);
                ui->pushButton_selectDirectory->setEnabled(1);
                ui->checkBox_deleteFiles->setEnabled(1);
                ui->radioButton_collisionFilename->setEnabled(1);
                ui->radioButton_collisionModify->setEnabled(1);
                ui->radioButton_iterateOnce->setEnabled(1);
                ui->radioButton_iterateTimer->setEnabled(1);

                ui->spinBox_3->setEnabled(1);


                ui->radioButton_operandAnd->setEnabled(1);
                ui->radioButton_operandOr->setEnabled(1);
                ui->radioButton_operandXor->setEnabled(1);
                ui->radioButton_operandNot->setEnabled(1);
                ui->radioButton_operandlShift->setEnabled(1);
                ui->radioButton_operandRshift->setEnabled(1);

                if (operationType != 3) {
                    if(operationType != 4 && operationType != 5) {
                        ui->groupBox->setEnabled(1);
                        ui->groupBox_2->setEnabled(1);
                    }
                    ui->groupBox_3->setEnabled(1);
                    ui->groupBox_5->setEnabled(1);
                }
                ui->lineEdit_masksAdd->setEnabled(1);
                ui->pushButton_masksAdd->setEnabled(1);


                //cleanup
                delete myTimer;
                files->clear();
                delete files;
                delete directory;

            });

        } else {
            this->modifyFile(directory, files);
            if(ui->checkBox_deleteFiles->isChecked()) {
                for (const QString& fileName : *files) {
                    // Construct the absolute file path
                    QString absoluteFilePath = directory->filePath(fileName);

                    // Remove the file
                    if (!QFile::remove(absoluteFilePath)) {
                        // Handle error if removal fails
                        QMessageBox::critical(nullptr, "Error", "Failed to delete file:" + absoluteFilePath);
                    }
                }
            }
        }
    } else {
        if(!isMaskPresent) {
            QMessageBox::information(nullptr, "Error", "Mask is missing!");
        } else {
            QMessageBox::information(nullptr, "Error", "Programm is running!");
        }

    }




}
void MainWindow::on_spinBox_3_valueChanged(int arg1)
{
    msSetTime = std::chrono::milliseconds(1000*arg1);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    byteOperand.resize(1, '\0');
    ui->groupBox_5->setEnabled(0);
    ui->spinBox_3->setEnabled(0);

}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::on_radioButton_iterateTimer_clicked()
{
    ui->spinBox_3->setEnabled(1);
    isTimered = 1;
}

void MainWindow::on_radioButton_iterateOnce_clicked()
{
    ui->spinBox_3->setEnabled(0);
    isTimered = 0;
}



void MainWindow::on_pushButton_masksAdd_clicked()
{
    if(ui->lineEdit_masksAdd->text().isEmpty()) {
        QMessageBox::information(nullptr, "Error", "Empty mask!");
        isMaskPresent = 0;
        return;
    }
    if(!isValidFileMask(ui->lineEdit_masksAdd->text())) {
        QMessageBox::critical(nullptr, "Error", "Invalid mask!");
        isMaskPresent = 0;
        return;
    }
    ui->label_maskSettingCurrentMask->setText(ui->lineEdit_masksAdd->text());
    isMaskPresent = 1;

}


void MainWindow::on_lineEdit_2_editingFinished()
{
    QDir directory(ui->lineEdit_2->text());
    if(!directory.exists()) {
        QMessageBox::critical(nullptr, "Error", "Invalid directory!");
        ui->lineEdit_2->setText("C:/");


    }
}



void MainWindow::on_pushButton_selectDirectory_clicked()
{
    QString initialDir = "C:/Safe folder";

    QString selectedDir = QFileDialog::getExistingDirectory(nullptr, "Select a directory", initialDir);
    if(selectedDir.isEmpty()) {
        ui->lineEdit_2->setText("C:/Safe folder");
    } else {
        ui->lineEdit_2->setText(selectedDir);
    }

}


void MainWindow::on_radioButton_operandAnd_clicked()
{

    if(operationType > 2 && operationType < 6) {
        isShiftOperand = false;
        ui->groupBox->setEnabled(1);
        ui->groupBox_2->setEnabled(1);
        ui->groupBox_5->setEnabled(0);
        ui->groupBox_3->setEnabled(1);
    }
    operationType = 0;

}


void MainWindow::on_radioButton_operandOr_clicked()
{

    if(operationType > 2 && operationType < 6) {
        isShiftOperand = false;
        ui->groupBox->setEnabled(1);
        ui->groupBox_2->setEnabled(1);
        ui->groupBox_5->setEnabled(0);
        ui->groupBox_3->setEnabled(1);
    }
    operationType = 1;
}


void MainWindow::on_radioButton_operandXor_clicked()
{

    if(operationType > 2 && operationType < 6) {
        isShiftOperand = false;
        ui->groupBox->setEnabled(1);
        ui->groupBox_2->setEnabled(1);
        ui->groupBox_5->setEnabled(0);
        ui->groupBox_3->setEnabled(1);
    }
    operationType = 2;
}


void MainWindow::on_radioButton_operandNot_clicked()
{

    if(operationType != 3 ) {
        isShiftOperand = false;
        ui->groupBox->setEnabled(0);
        ui->groupBox_2->setEnabled(0);
        ui->groupBox_5->setEnabled(0);
        ui->groupBox_3->setEnabled(0);
    }
    operationType = 3;
}


void MainWindow::on_radioButton_operandlShift_clicked()
{

    if(operationType < 4) {
        isShiftOperand = true;
        ui->groupBox->setEnabled(0);
        ui->groupBox_2->setEnabled(0);
        ui->groupBox_5->setEnabled(1);
        ui->groupBox_3->setEnabled(1);
    }
    operationType = 4;

}


void MainWindow::on_radioButton_operandRshift_clicked()
{

    if(operationType < 5) {
        isShiftOperand = true;
        ui->groupBox->setEnabled(0);
        ui->groupBox_2->setEnabled(0);
        ui->groupBox_5->setEnabled(1);
        ui->groupBox_3->setEnabled(1);
    }
    operationType = 5;
}










void MainWindow::on_spinBox_valueChanged(int arg1)
{

    byteOperand.resize(arg1, convertToByte(ui->lineEdit->text()));

    if(byteCount >= byteOperand.size()) {
        if(byteCount > innerByteCount) {
            qDebug() << "1\n";
            byteCount = byteOperand.size() - 1;
            innerByteCount--;
            if(byteOperand.size() == 1) {
                innerByteCount = 0;
            }
        } else if (innerByteCount >= byteOperand.size()) {
            qDebug() << "2\n";
            innerByteCount = byteOperand.size() - 1;
            if(!innerByteCount) {
                byteCount = 0;
            }
        }
    }
    ui->lineEdit_3->setText(charToBinary(byteOperand[innerByteCount]));
    qDebug() << "Inner: " << innerByteCount << " byteCount: "<< byteCount;
    QString tempStr = "Byte#";
    tempStr.append(QString::number(innerByteCount));
    ui->label_byteNumber->setText(tempStr);

}


void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    shiftCount = arg1;
}











void MainWindow::on_pushButton_bitMaskButton_clicked()
{

    if(ui->lineEdit_3->text().size() == 8) {
        qDebug() << innerByteCount << "  " << byteCount;
        byteOperand[innerByteCount] = convertToByte(ui->lineEdit_3->text());
        innerByteCount++;
        byteCount = innerByteCount;

        if(innerByteCount >= byteOperand.size()) {
            innerByteCount = 0;
        }
        QString tempStr = "Byte#";
        tempStr.append(QString::number(innerByteCount));
        ui->label_byteNumber->setText(tempStr);
        qDebug() << innerByteCount << "  " << byteCount;
        ui->lineEdit_3->setText(charToBinary(byteOperand[innerByteCount]));
    }
    qDebug() << "Inner: " << innerByteCount << " byteCount: "<< byteCount;
}


void MainWindow::on_lineEdit_3_textChanged(const QString &arg1)
{
    if(arg1.size()!= 8) {
        ui->lineEdit_3->setProperty("state", "0");
    } else {
        ui->lineEdit_3->setProperty("state", "1");
    }
    style()->polish(ui->lineEdit_3);
}


void MainWindow::on_radioButton_collisionFilename_clicked()
{
    collisionType = 0;
}


void MainWindow::on_radioButton_collisionModify_clicked()
{
    collisionType = 1;
}




