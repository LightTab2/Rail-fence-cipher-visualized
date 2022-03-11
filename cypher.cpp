#include "mainwindow.h"
#include "ui_mainwindow.h"

//Dwie poniższe funckje zwracają znak z szyfrowania/deszyfrowania
void MainWindow::cypherStep()
{
    unsigned    height1     = ui->firstBoardHeight  ->value(),
                height2     = ui->secondBoardHeight ->value(),
                maxHeight   = std::max(height1, height2);

    unsigned    textSize    = ui->firstText->toPlainText().size();
    bool        inverted    = ui->inverseCheck->isChecked();

    if (iteration < textSize)
    {
        //Przykład:
        //    *
        //  *   *
        //*
        //Liczba indeksów zajmowanych przez taki płotek wynosi (height * 2 - 2), ale w przypadku dwóch płoktów, to wygląda inaczej:
        //      *
        //    *   *       x
        //  *       *   x   x
        //*           ?
        unsigned fenceSize = ((height1 * 2 - 1 - /*(height1 > 1)*/ 1) + (height2 * 2 - /*(height2 > 0)*/1 - /*(height2 > 1)*/1));
        unsigned index = iteration % fenceSize,
                pos;

        if (index < height1)
            pos = /*inverted ? (((height2 > height1) ? (height2 - height1) : 0) + (height1 - index - 1)) :*/ index;
        else if (height1 != 1 && index < (height1 * 2 - (height1 > 1)))
        {
            index -= height1 - 1;
            pos = /*inverted ? (((height2 > height1) ? (height2 - height1) : 0) + index) : */(height1 - index - 1);
        }
        else if (index < ((height1 * 2 - 1 - (height1 > 1)) + height2))
        {
            index -= height1 * 2 - (/*(height1 == 1) ? 1 : */2);
            pos = /*inverted ? (maxHeight - index - 1) : */index;
        }
        else
        {
            index -= (height1 * 2) + (height2 - 1) - (/*(height1 == 1) ? 1 : */2);
            pos = /*inverted ? (((height1 > height2) ? (height1 - height2) : 0) + index) : */(height2 - index - 1);
        }
        ui->visTable->setItem((inverted ? (maxHeight - 1 - pos) : pos), iteration, new QTableWidgetItem(text.at(iteration)));

    }
    else if (iteration < (textSize + maxHeight))
    {
        unsigned    row = iteration - textSize,
                    count = 0;
        //Kopiowanie wierszy, tak żeby był znak po znaku
        for (unsigned i = 0u; i != textSize; ++i)
        {
            QTableWidgetItem *cell = ui->visTable->item(row, i);
            if (cell)
                ui->visTable->setItem(maxHeight + row + 1, count++, new QTableWidgetItem(cell->text()));
        }
    }
    else
    {
        unsigned    row = iteration - textSize,
                    count = processedText.size();
        //Kopiowanie wierszy do przetworzonego tekstu
        for (unsigned i = 0u; i != textSize; ++i)
        {
            QTableWidgetItem *cell = ui->visTable->item(row + 1, i);

            if (cell)
            {
                ui->visTable->setItem(maxHeight * 2 + 2, count++, new QTableWidgetItem(cell->text()));
                processedText += cell->text();
            }
        }
    }
}

void MainWindow::decypherStep()
{
    unsigned    height1     = ui->firstBoardHeight  ->value(),
                height2     = ui->secondBoardHeight ->value(),
                maxHeight   = std::max(height1, height2);

    unsigned    textSize    = ui->firstText->toPlainText().size();
    unsigned    fenceSize   = (height1 * 2 - 2) + (height2 * 2 - 2);

    bool        inverted    = ui->inverseCheck->isChecked();

    if (iteration < std::max(height1, height2))
    {
        unsigned  mode = 0;
        //,,To ja wysłałem go do walki ze Śniącym, to ja sporwadzę go teraz z powrotem''
        unsigned magicBarrier = std::min(height1, height2) - 1;
        if (iteration == magicBarrier)
            mode = 1;
        else if (iteration > magicBarrier)
            mode = 2;

        unsigned cycles     = textSize / fenceSize,
                 letterCount;
        //wierzchołki, które przetwarza się specjalnie
        if (iteration == 0)
        {
            letterCount = cycles * 2;

            if (textSize % fenceSize)
                ++letterCount;

            if ((textSize % fenceSize) >= (height1 * 2 - 1))
                ++letterCount;
        }
        else if (iteration == (maxHeight - 1))
            letterCount = text.size();
        else if (mode == 0)
        {
            letterCount = 4 * cycles;
            if ((textSize % fenceSize) >= (iteration + 1))
                ++letterCount;

            if ((textSize % fenceSize) >= ((2 * height1 - 1) - iteration))
                ++letterCount;

            if ((textSize % fenceSize) >= ((2 * height1 - 1) + iteration))
                ++letterCount;

            if ((textSize % fenceSize) >= ((2 * height1 - 2) + (2 * height2 - 1) - iteration))
                ++letterCount;
        }
        else if (mode == 1)
        {
            letterCount = 3 * cycles;
            if ((textSize % fenceSize) >= (iteration + 1))
                ++letterCount;

            if (height1 > height2)
            {
                if ((textSize % fenceSize) >= (2 * height1 - 1) - iteration)
                    ++letterCount;
            }
            else
            {
                if ((textSize % fenceSize) >= (2 * height1 - 2) + (2 * height2 - 1) - iteration)
                    ++letterCount;
            }
            if ((textSize % fenceSize) >= (2 * height1 - 1) + iteration)
                ++letterCount;
        }
        else
        {
            letterCount = 2 * cycles;

            if (height1 > height2)
            {
                if ((textSize % fenceSize) >= (iteration + 1))
                    ++letterCount;

                if ((textSize % fenceSize) >= ((2 * height1 - 1) - iteration))
                    ++letterCount;
            }
            else
            {
                if ((textSize % fenceSize) >= ((2 * height1 - 1) + iteration))
                    ++letterCount;

                if ((textSize % fenceSize) >= ((2 * height1 - 2) + (2 * height2 - 1) - iteration))
                    ++letterCount;
            }

        }

        unsigned deletePos = (inverted ? (text.size() - letterCount) : 0);
        for (unsigned i = 0u; i != letterCount; ++i)
        {
            ui->visTable->setItem((inverted ? (maxHeight - iteration - 1) : iteration), i, new QTableWidgetItem(text.at(deletePos)));
            text.remove(deletePos, 1);
        }
    }
    else if (iteration < (textSize + maxHeight))
    {
        unsigned index = (iteration - maxHeight) % fenceSize,
                pos;
        if (index < height1)
            pos = index;
        else if (height1 != 1 && index < (height1 * 2 - (height1 > 1)))
        {
            index -= height1 - 1;
            pos = height1 - index - 1;
        }
        else if (index < ((height1 * 2 - 1 - (height1 > 1)) + height2))
        {
            index -= height1 * 2 - 2;
            pos = index;
        }
        else
        {
            index -= (height1 * 2) + (height2 - 1) - 2;
            pos = height2 - index - 1;
        }
        if (inverted)
            pos = maxHeight - pos - 1;
        QString copiedChar = ui->visTable->item(pos, posArray[pos]++)->text();
        ui->visTable->setItem(pos + maxHeight + 1, iteration - maxHeight, new QTableWidgetItem(copiedChar));
        ui->visTable->setItem(2 * maxHeight + 2, iteration - maxHeight, new QTableWidgetItem(copiedChar));
        processedText += copiedChar;
    }
}

void MainWindow::execute()
{
    //Podczas działania programu, zabraniamy modyfikować CheckBox ani SpinBox
    ui->modeCheck           ->setEnabled(false);
    ui->inverseCheck        ->setEnabled(false);
    ui->firstBoardHeight    ->setEnabled(false);
    ui->secondBoardHeight   ->setEnabled(false);
    ui->firstText           ->setEnabled(false);

    //Zmieniamy funkcjonalność przycisku na stop
    ui->executeButton->setText("Stop");
    ui->executeButton->disconnect();
    connect(ui->executeButton, &QPushButton::clicked, this, &MainWindow::stopExecuting);

    //Najpierw trzeba posprzątać po poprzednim uruchomieniu programu, o ile to się wydarzyło
    ui->secondText->clear();
    ui->visTable->setColumnCount(0);
    ui->visTable->setRowCount(0);
    ui->visProgressBar->setValue(0);
    iteration = 0;

    //Ustawiamy maksymalne iteracje programu na liczbę znaków w naszym tekście
    unsigned    height1 = ui->firstBoardHeight->value(),
                height2 = ui->secondBoardHeight->value();

    maxIterations = (ui->firstText->toPlainText().size()) + std::max(height1, height2)*(cypherMode ? 1 : 2);
    ui->visProgressBar->setMaximum(std::max(1u, maxIterations));

    //Ustawiamy strukturę do wizualizacji
    ui->visTable->setRowCount((std::max(height1, height2) * 2) + 3);
    ui->visTable->setColumnCount(ui->firstText->toPlainText().size());
    ui->visTable->update();
    if (!maxIterations)
        return;

    //Zbieramy tekst do przetworzenia, który zawsze znajduje się w tym samym polu tekstowym w programie
    text            = ui->firstText->toPlainText();
    processedText   = "";

    posArray = new unsigned[std::max(height1, height2)];
    memset(posArray, 0, sizeof(unsigned) * std::max(height1, height2));

    //Timer, który wywołuje executing co określony czas
    executeTimer.start();
}

//Funkcja wywoływana w określonych interwałach
void MainWindow::executing()
{
    if (cypherMode)
        decypherStep();
    else
        cypherStep();

    //Aktualizacja paska postępu
    ui->visProgressBar->setValue(++iteration);
    ui->secondText->setPlainText(processedText);

    if (iteration == maxIterations)
        stopExecuting();
}

void MainWindow::stopExecuting()
{
    executeTimer.stop();

    //Przywracamy funkcjonalność przycisku
    ui->executeButton->setText(cypherMode ? "Deszyfruj" : "Szyfruj");
    ui->executeButton->disconnect();
    connect(ui->executeButton, &QPushButton::clicked, this, &MainWindow::execute);

    //Przypisujemy przetworzony tekst do pola na wyjście programu
    ui->secondText->setPlainText(processedText);

    //Koniec roboty, więc z powrotem pozwalamy zmieniać opcje szyfrowania
    ui->modeCheck           ->setEnabled(true);
    ui->inverseCheck        ->setEnabled(true);
    ui->firstBoardHeight    ->setEnabled(true);
    ui->secondBoardHeight   ->setEnabled(true);
    ui->firstText           ->setEnabled(true);

    if (cypherMode)
        delete[] posArray;
}
