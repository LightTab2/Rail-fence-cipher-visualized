#include "mainwindow.h"
#include "ui_mainwindow.h"

//Dwie poniższe funckje zwracają znak z szyfrowania/deszyfrowania
void MainWindow::cypherStep()
{
    //Wysokości płotków
    unsigned    height1     = ui->firstBoardHeight  ->value(),
                height2     = ui->secondBoardHeight ->value(),
                maxHeight   = std::max(height1, height2),

                textSize    = ui->firstText->toPlainText().size();
    bool        inverted    = ui->inverseCheck->isChecked();    //Czy płotek rysujemy ,,do góry nogami''

    //Musimy najpierw przetworzyć cały tekst
    if (iteration < textSize)
    {
        //Ustawienie nowego interwału kroku, jeżeli użytkownik zmienił prędkość wizualizacji podczas jej trwania
        executeTimer.setInterval(ui->drawSpeed->value());
        //Przykład jednego płotku (powtarza się cyklicznie, jeżeli jest taka potrzeba):
        //*
        //  *   *
        //    *
        //Liczba indeksów zajmowanych przez taki płotek wynosi [height * 2 - 2]

        //*           x
        //  *       *   x   x
        //    *   *       x
        //      *
        //* - płot o wysokości 4, x - płot o wysokości 3
        //Jak widać, to są tak naprawdę dwa płoty nałożone na siebie, więc skoro:
        //Jeden płot o wysokości [height] ma rozmiar [height * 2 - 2], a mamy złączenie dwóch, to
        //Jeżeli ich wysokości to odpowiednio: [height1] i [height2], to wzór na rozmiar złączonych płotków to po prostu:
        //[(height1 * 2 - 2) + (height2 * 2 - 2)]

        //Ominąłem w tej logice ważne założenie: płotki muszą być wysokości >= 2.
        //
        //Na początku próbowałem pozwolić na wysokości 0 i 1 (taka wysokość posiada dwuznaczną interpretację, więc wybrałem jedną z nich),
        //ale szybko kod stał się bardzo nieczytelny i wtedy zacząłem myśleć:
        //Po pierwsze: nie ma sensu ustawiać wysokości na 0, ponieważ jeżeli chcemy, aby drugiego płotka nie było, to jest to równoważne z ustawieniem
        //jego wysokości na wysokość płotka pierwszego tzn. [height1 = height2]
        //Po drugie: wysokość 1 sprowadza się do przekopiowania tekstu, co nie jest żadnym szyfrem, a gdy tylko jeden z płotków posiada wysokość 1, to nie wiadomo czy:
        //- mamy powtarzać wierzchołki
        //- powtarzać tylko jeden z wierzchołków
        //- nic nie robić
        //Dlatego postanowiłem stworzyć założenie wysokości >= 2, co także uproszcza powyższe rozumowanie, bo nie musimy rozważać 9 różnych kombinacji wysokość >= 2, wysokość = 1, wysokość = 0 (kolejność ma znaczenie).
        unsigned    fenceSize1  = (height1 * 2 - 2),
                    fenceSize2  = (height2 * 2 - 2),
                    fenceSize   = fenceSize1 + fenceSize2,
                    index       = iteration % fenceSize,                    //[iteracja] modulo [rozmiar_płotka] daje nam index kawałka drewna użytego do zbudowania płotka
                                                                            //Modulo jest po to, żeby uwzględnić cykliczne powtarzanie się wzoru płotka
                    pos;

        //Przykład:
        //0 | *
        //1 |   *
        //2 |     *
        //3 |       *
        //h1 = 4, h2 = 3; x - już narysowane, * - w trakcie rysowania
        if (index < height1)
            pos = index;
        //Przykład:
        //0 | x
        //1 |   x       *
        //2 |     x   *
        //3 |       x
        //h1 = 4, h2 = 3; x - już narysowane, * - w trakcie rysowania
        else if (index < fenceSize1)
        {
            //Patrzymy na indeks względem wierzchołka ([x] w szkicu poniżej)
            index -= height1;
            //I liczymy od końca tzn.:
            //^ height - 4 ----- 0 |       *
            //| height - 3 ----- 1 |     *
            //| height - 2 ----- 2 |   *
            //| height - 1 ----- 3 | x       ----- wierzchołek, którego nie bierzemy pod uwagę, bo został narysowany dla przypadku [index < height1]
            //| - Idziemy od dołu do góry
            //[index] rośnie od 0, więc jeżeli chcemy zacząć rysowanie od height - 2, to musimy jeszcze odjąć 2
            pos = (height1 - index) - 2;
        }
        else if (index < (fenceSize1 + height2))
        {
            index -= fenceSize1;
            pos = index;
        }
        else
        {
            //Analogicznie, patrzymy na indeks względem wierzchołka, tym razem dla 2. płotku
            index -= (fenceSize1 + height2);
            //Idziemy w drugą stronę, tak jak w poprzednim przypadku
            pos = (height2 - index) - 2;
        }
        QTableWidgetItem *newItem = new QTableWidgetItem(text.at(iteration));
        newItem->setBackground(QBrush(QColor(255,255,255)));
        //Odwracanie dzieje się tutaj
        ui->visTable->setItem((inverted ? (maxHeight - 1 - pos) : pos), iteration, newItem);

    }
    else if (iteration < (textSize + maxHeight))
    {
        //Ustawienie nowego interwału kroku, jeżeli użytkownik zmienił prędkość wizualizacji podczas jej trwania
        executeTimer.setInterval(ui->mergeSpeed->value());

        unsigned    row = iteration - textSize,
                    count = 0;
        //Kopiowanie wierszy, tak żeby był znak po znaku
        for (unsigned i = 0u; i != textSize; ++i)
        {
            QTableWidgetItem *cell = ui->visTable->item(row, i);
            if (cell)
            {
                QTableWidgetItem *newItem = new QTableWidgetItem(cell->text());
                newItem->setBackground(QBrush(QColor(240, 185, 0)));
                ui->visTable->setItem(maxHeight + row + 1, count++, newItem);
            }
        }
    }
    else
    {
        //Ustawienie nowego interwału kroku, jeżeli użytkownik zmienił prędkość wizualizacji podczas jej trwania
        executeTimer.setInterval(ui->mergeSpeed->value());

        unsigned    row = iteration - textSize,
                    count = processedText.size();
        //Kopiowanie wierszy do przetworzonego tekstu
        for (unsigned i = 0u; i != textSize; ++i)
        {
            QTableWidgetItem *cell = ui->visTable->item(row + 1, i);

            if (cell)
            {
                QTableWidgetItem *newItem = new QTableWidgetItem(cell->text());
                newItem->setBackground(QBrush(QColor(0, 255, 0)));
                ui->visTable->setItem(maxHeight * 2 + 2, count++, newItem);
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
    unsigned    fenceSize1  = (height1 * 2 - 2),
                fenceSize2  = (height2 * 2 - 2),
                fenceSize   = fenceSize1 + fenceSize2;

    bool        inverted    = ui->inverseCheck->isChecked();

    //Odwrotne kroki do szyfrowania - najpierw znajdujemy rzędy, z których powstał szyfr
    if (iteration < maxHeight)
    {
        //Ustawienie nowego interwału kroku, jeżeli użytkownik zmienił prędkość wizualizacji podczas jej trwania
        executeTimer.setInterval(ui->mergeSpeed->value());

        //Trzeba przetworzyć 5 przypadków:
        //x               *         ----- 1. w tej linii znajdują się wierzchołki obu płotów
        //  x           x   *   *   ----- 2. w tej linii mamy elementy obu płotów bez wierzchołków
        //    x       x       *     ----- 3. w tej linii są elementy dłuższego płotu, ale też wierzchołki
        //      x   x               ----- 4. w tej linii znajdują się elementy dłuszego płotku, ale nie ma elementów krótszego
        //        x                 ----- 5. w tej linii znajdują się same wierzchołki dłuszego płotu

        //Celem jest wyznaczenie liczby liter, które były na poszczególnych ,,szczeblach'' płotu
        unsigned    mode            = 0,
                    magicBarrier    = std::min(height1, height2) - 1; //Ta zmienna pozwala rozróżnić przypadki 2., 3., 4. Jest to numer rzędu, w którym występuje przypadek 3., więć wiemy, że powyżej jest przypadek 2., a poniżej 4.

        if (iteration == magicBarrier)
            mode = 1;
        else if (iteration > magicBarrier)
            mode = 2;

        unsigned    cycles     = textSize / fenceSize,
                    letterCount;

        //Przypadek 1.
        if (iteration == 0)
        {
            letterCount = cycles * 2;

            if (textSize % fenceSize)
                ++letterCount;

            if ((textSize % fenceSize) > fenceSize1)
                ++letterCount;
        }
        else if (iteration == (maxHeight - 1))
            letterCount = text.size();
        else if (mode == 0)
        {
            letterCount = 4 * cycles;
            if ((textSize % fenceSize) > iteration)
                ++letterCount;

            if ((textSize % fenceSize) > (fenceSize1 - iteration))
                ++letterCount;

            if ((textSize % fenceSize) > (fenceSize1 + iteration))
                ++letterCount;

            if ((textSize % fenceSize) > (fenceSize - iteration))
                ++letterCount;
        }
        else if (mode == 1)
        {
            letterCount = 3 * cycles;
            if ((textSize % fenceSize) > iteration)
                ++letterCount;

            if (height1 > height2)
            {
                if ((textSize % fenceSize) > (fenceSize1 - iteration))
                    ++letterCount;
            }
            else
            {
                if ((textSize % fenceSize) > fenceSize - iteration)
                    ++letterCount;
            }
            if ((textSize % fenceSize) > (fenceSize1 + iteration))
                ++letterCount;
        }
        else
        {
            letterCount = 2 * cycles;

            if (height1 > height2)
            {
                if ((textSize % fenceSize) > iteration)
                    ++letterCount;

                if ((textSize % fenceSize) > (fenceSize1 - iteration))
                    ++letterCount;
            }
            else
            {
                if ((textSize % fenceSize) > (fenceSize1 + iteration))
                    ++letterCount;

                if ((textSize % fenceSize) > (fenceSize - iteration))
                    ++letterCount;
            }

        }

        //Przykład:
        //a        e        i   k
        //  b   d    f    h   j   l
        //    c        g
        //1. aeik   (4 litery)
        //2. bdfhjl (6 liter)
        //3. cg     (2 liter)
        //Zostanie złączone w: aeikbdfhjlcg
        //Widać, że powinniśmy czytać od początku i wkleić najpierw wiersz nr 1. a potem kolejne
        //Odwrócony:
        //    c           i
        //  b   d   f   h   j   l
        //a       e   g       k
        //1. ci     (2 liter)
        //2. bdfhjl (6 liter)
        //3. aegk   (4 litery)
        //Możemy wykorzystać fakt, że liczba liter posiada odwrotną kolejność i po prostu w odwróconym płotkowym,
        //zaczyąć od stawiania wierszy od końca i ,,wyrywać'' litery z końca przetworzonego tekstu, a nie początku
        unsigned deletePos = (inverted ? (text.size() - letterCount) : 0);
        for (unsigned i = 0u; i != letterCount; ++i)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(text.at(deletePos));
            newItem->setBackground(QBrush(QColor(240, 185, 0)));
            ui->visTable->setItem((inverted ? (maxHeight - iteration - 1) : iteration), i, newItem);
            text.remove(deletePos, 1);
        }
    }
    //Z powrotem układanie płotku i od razu przepisywanie zdeszyfrowanego tekstu
    else if (iteration < (textSize + maxHeight))
    {
        //Ustawienie nowego interwału kroku, jeżeli użytkownik zmienił prędkość wizualizacji podczas jej trwania
        executeTimer.setInterval(ui->drawSpeed->value());
        unsigned index = (iteration - maxHeight) % fenceSize,
                pos;
        if (index < height1)
            pos = index;
        else if (index < fenceSize1)
        {
            index -= height1;
            pos = (height1 - index) - 2;
        }
        else if (index < (fenceSize1 + height2))
        {
            index -= fenceSize1;
            pos = index;
        }
        else
        {
            index -= (fenceSize1 + height2);
            pos = (height2 - index) - 2;
        }

        if (inverted)
            pos = maxHeight - pos - 1;

        QString copiedChar = ui->visTable->item(pos, posArray[pos]++)->text();
        QTableWidgetItem *newItem = new QTableWidgetItem(copiedChar);
        newItem->setBackground(QBrush(QColor(255, 255, 255)));
        //Wrzuca znak do wizualizacji płotka
        ui->visTable->setItem(pos + maxHeight + 1, iteration - maxHeight, newItem);
        //Wrzuca znak do przetworzonego tekstu
        newItem = new QTableWidgetItem(copiedChar);
        newItem->setBackground(QBrush(QColor(0, 255, 0)));
        ui->visTable->setItem(2 * maxHeight + 2, iteration - maxHeight, newItem);
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
    unsigned    height1     = ui->firstBoardHeight->value(),
                height2     = ui->secondBoardHeight->value(),
                maxHeight   = std::max(height1, height2),
                textSize    = ui->firstText->toPlainText().size();

    maxIterations = textSize + maxHeight * (cypherMode ? 1 : 2);
    ui->visProgressBar->setMaximum(std::max(1u, maxIterations));

    if (!maxIterations)
        return;

    //Ustawiamy strukturę do wizualizacji
    ui->visTable->setRowCount(maxHeight * 2 + 3);
    ui->visTable->setColumnCount(textSize);
    ui->visTable->update();

    //,,Linie'' oddzielające odpowiednie części wizualizacji
    for (unsigned i = 0u; i != textSize; ++i)
    {
        QTableWidgetItem *newItem = new QTableWidgetItem('\0');
        newItem->setBackground(QBrush(QColor(114,180,180)));
        ui->visTable->setItem(maxHeight, i, newItem);
        newItem = new QTableWidgetItem('\0');
        newItem->setBackground(QBrush(QColor(114,180,180)));
        ui->visTable->setItem(maxHeight * 2 + 1, i, newItem);
    }
    //Zbieramy tekst do przetworzenia, który zawsze znajduje się w tym samym polu tekstowym w programie
    text            = ui->firstText->toPlainText();
    processedText   = "";

    posArray        = new unsigned[maxHeight];
    memset(posArray, 0, sizeof(unsigned) * maxHeight);

    //Timer, który wywołuje executing co określony czas
    bool        inverted    = ui->inverseCheck->isChecked();    //Czy płotek rysujemy ,,do góry nogami''
    inverted ? executeTimer.setInterval(ui->mergeSpeed->value()) : executeTimer.setInterval(ui->mergeSpeed->value());
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
