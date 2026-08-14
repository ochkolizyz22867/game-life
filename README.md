README (Русский)
Evolution Life (C++)

Это консольная игра “эволюция клеток” на C++: поле клеточного автомата напоминает Game of Life, но у каждой клетки есть геном (набор параметров), который влияет на то, как выглядит клетка и откуда “наследуется” при размножении.
Как работает симуляция
Клетки и геном

Каждая клетка хранит:

    alive — живая/мёртвая клетка
    Genome:
        reproduction_speed — скорость размножения
        resistance — устойчивость
        immunity — иммунитет

Для выживших/рождающихся клеток геном используется:

    при визуализации (цвет зависит от среднего значения генов)
    как “приспособленность” (fitness) при выборе родителя генома

Правила “жизни” (как в Game of Life)

Для каждой клетки считается число живых соседей (8 соседей, включая диагонали) с тороидальным замыканием краёв.

Функция tick() применяет правила:

    Если клетка жива:
        выживает, если count == 2 или count == 3
        иначе умирает
    Если клетка мертва:
        оживает, если `count == 3

Наследование генома при рождении

В updateCells():

    Если клетка должна стать живой (next_state == 1):
        если она была живой раньше — геном сохраняется
        если она была мёртвой и “родилась” — геном берётся от ближайшего (соседнего) живого родителя:
            среди соседей (в радиусе 1 клетки) выбирается клетка с максимальным fitness
            fitness = reproduction_speed + resistance + immunity
            геном потомка создаётся через мутацию

    Если клетка становится мёртвой — геном сбрасывается в пустое значение.

Мутации

Ген потомка создаётся функцией copyWithMutation().

Мутация применяется так:

    к каждому параметру генома применяется множитель вида:
        param *= (1 + delta)
        где delta случайное число из диапазона [-gene_mutation_range, +gene_mutation_range]
    gene_mutation_range по умолчанию = `0.1f

Итог: параметры обычно меняются примерно на ±10% (в рамках этой случайности).
Визуализация

Экран обновляется в консоли (ASCII-символы).

    Живая клетка рисуется как @ цветом, который зависит от среднего значения генома:
        среднее avg = (reproduction_speed + resistance + immunity) / 3
        пороги:
            avg < 0.8 → красный
            avg < 1.2 → зелёный
            avg < 1.6 → голубой
            avg < 2.0 → жёлтый
            иначе → фиолетовый
    Мёртвые клетки: .

Учтите

Это консольная игра: используются ANSI-коды цвета. Иногда в некоторых терминалах цвета могут не сработать.
Управление

У игры управления нет — она крутится в бесконечном цикле.
Сборка и запуск
Linux / macOS (пример)
bash

g++ -std=c++17 -O2 main.cpp -o evo_life
./evo_life

Windows (пример)

Если используете MinGW:
bash

g++ -std=c++17 -O2 main.cpp -o evo_life.exe
evo_life.exe

Важные замечания по текущей реализации (что можно улучшить)

    system("cls") — это команда для Windows; на Linux/macOS понадобится другой вызов (clear).
    В коде используется глобальная функция srand(time(0)), но дальше — генератор в copyWithMutation() и начальная генерация на отдельном default_random_engine; srand() может быть лишним.
    Логика board[row][col].second = {}; для Genome формально должна быть валидной для вашей структуры (в текущем виде Genome — POD, но “пустое” значение не всегда очевидно). Если захотите — лучше явно хранить нейтральный геном или флаг.

README (English)
Evolution Life (C++)

This is a console “evolution” simulation in C++: the world is based on a Game of Life-like cellular automaton, but each cell has a genome (a set of parameters). When new cells are born, their genome is inherited from nearby “parents” with added mutation.
Simulation rules
Cell state and genome

Each cell stores:

    alive — whether the cell is alive
    Genome:
        reproduction_speed
        resistance
        immunity

The genome is used for:

    choosing a color for visualization (based on the average genome value)
    selecting the parent at birth time using a fitness score

Life / death (Game of Life style)

For each cell, the simulation counts the number of alive neighbors (8-neighborhood). Edge wrapping is toroidal (cells at borders “see” cells on the opposite side).

tick() rules:

    If the cell is alive:
        survives if count == 2 or count == 3
        otherwise it dies
    If the cell is dead:
        becomes alive if count == 3

Genome inheritance on birth

Inside updateCells():

    If next_state == 1 (cell will be alive):
        if the cell was already alive — its genome is kept
        if it was dead and is born — its genome is copied from the best nearby living parent:
            only neighbors in radius 1 (8 cells) are considered
            parent selection uses:
                fitness = reproduction_speed + resistance + immunity
            the offspring genome is created via mutation

    If next_state == 0 — the genome is reset.

Mutation model

copyWithMutation() mutates each gene by multiplying it with:

    param *= (1 + delta)
    delta is sampled uniformly from [-gene_mutation_range, +gene_mutation_range]
    gene_mutation_range = 0.1f by default

So each parameter typically changes by about ±10% (within the random sampling range).
Visualization

Each generation is printed to the console using ASCII and ANSI colors.

    Alive cells are displayed as @ with a color depending on:
        avg = (reproduction_speed + resistance + immunity) / 3
    Thresholds:
        avg < 0.8 → red
        avg < 1.2 → green
        avg < 1.6 → blue
        avg < 2.0 → yellow
        otherwise → purple
    Dead cells are shown as .

Controls

No user controls. The simulation runs forever.
Build and run
Linux / macOS (example)
bash

g++ -std=c++17 -O2 main.cpp -o evo_life
./evo_life

Windows (example)

With MinGW:
bash

g++ -std=c++17 -O2 main.cpp -o evo_life.exe
evo_life.exe

Notes / potential improvements

    system("cls") is Windows-specific; on Linux/macOS you’d use clear.
    The initialization and random usage could be unified under one RNG for consistency.
    Resetting Genome with {} may be better replaced with explicit values or a separate “valid genome” flag.
