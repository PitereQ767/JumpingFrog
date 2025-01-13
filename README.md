# JumpingFrog

JumpingFrog to gra napisana w języku C, w której gracz steruje żabą skaczącą po liściach na stawie. Celem gry jest dotarcie do końca planszy, unikając przeszkód i zbierając punkty.

## Wprowadzenie
JumpingFrog to prosta, ale wciągająca gra, która sprawdzi Twoje umiejętności zręcznościowe i refleks. Gra została stworzona jako projekt na zajęcia z Podstaw Programowania.

## Wymagania
Aby uruchomić grę, potrzebujesz:
- Kompilatora języka C (np. GCC)
- Systemu operacyjnego zgodnego z POSIX (np. Linux, macOS) lub Windows z zainstalowanym MinGW

## Instalacja
1. Sklonuj repozytorium:
    ```sh
    git clone https://github.com/twoje-repozytorium/JumpingFrog.git
    ```
2. Przejdź do katalogu z grą:
    ```sh
    cd JumpingFrog
    ```
3. Skompiluj kod źródłowy:
    ```sh
    gcc -o JumpingFrog main.c -lncurses
    ```

## Uruchomienie gry
Aby uruchomić grę, wykonaj poniższe polecenie w terminalu:
```sh
./JumpingFrog
```

## Zasady gry
- Sterowanie żabą odbywa się za pomocą klawiszy strzałek.
- Celem gry jest dotarcie do końca planszy, skacząc po liściach.
- Unikaj przeszkód, takich jak kamienie i węże.
- Zbieraj punkty, skacząc na specjalne liście.

## Opis gry

JumpingFrog to dynamiczna gra zręcznościowa, w której gracz wciela się w żabę skaczącą po liściach na stawie. Gra oferuje różnorodne poziomy trudności, które sprawdzą refleks i umiejętności gracza. Każdy poziom jest pełen przeszkód, takich jak kamienie i węże, które gracz musi ominąć, aby dotrzeć do końca planszy. Dodatkowo, na planszy znajdują się specjalne liście, które dają graczowi dodatkowe punkty. Gra kończy się, gdy gracz dotrze do końca planszy lub straci wszystkie życia.

## Biblioteka ncurses

JumpingFrog wykorzystuje bibliotekę ncurses do obsługi interfejsu tekstowego. Ncurses to biblioteka programistyczna, która umożliwia tworzenie interaktywnych aplikacji tekstowych w terminalu. Dzięki ncurses, gra może wyświetlać dynamiczne elementy, takie jak poruszające się przeszkody i animacje skoków żaby, bez potrzeby korzystania z graficznego interfejsu użytkownika.

Ncurses jest idealnym rozwiązaniem dla gier tekstowych, takich jak JumpingFrog, ponieważ umożliwia tworzenie złożonych interfejsów użytkownika w środowisku terminalowym. Dzięki tej bibliotece, gra może działać na różnych systemach operacyjnych, które obsługują terminale POSIX, takich jak Linux i macOS, a także na Windowsie z zainstalowanym MinGW.

