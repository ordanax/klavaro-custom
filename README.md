# Klavaro Custom

Форк **Klavaro 3.14** — свободного клавиатурного тренажёра (GPL v3), с добавленной системой анализа и исправления ошибок печати.

A fork of **Klavaro 3.14** — a free touch typing tutor (GPL v3), extended with an error-analysis and error-correction training system.

---

## / Сделанные изменения / What's new

### 1. Детальное логирование ошибок / Detailed error logging

Каждая совершённая ошибка (нажатая клавиша вместо ожидаемой) записывается в отдельный файл. Every missed keystroke (pressed key instead of the expected one) is recorded to a dedicated file.

- Формат: символ, UTF-8 код, сколько раз ошиблись, сколько раз нажали верно, общее число попыток, процент ошибок
- Format: character, UTF-8 code, wrong count, correct count, total attempts, error rate
- Файл: `~/.local/share/klavaro/error_detail_<layout>.log`
- File: `~/.local/share/klavaro/error_detail_<layout>.log`

### 2. Анализ Парето (правило 80/20) / Pareto analysis (80/20 rule)

Автоматически определяет набор символов, на которые приходится **80% всех ошибок** — именно их и стоит тренировать в первую очередь. Определения rank a set of characters causing **80% of all errors** — those are the ones worth training first.

- Файл: `~/.local/share/klavaro/pareto_analysis_<layout>.log`
- File: `~/.local/share/klavaro/pareto_analysis_<layout>.log`

### 3. Новый режим — «Работа над ошибками» / New mode — "Error Practice"

Добавлен новый тип урока `Error Practice` (кнопка в главном окне). Режим генерирует тренировочный текст из **реального словаря языка** (например `ru.words`), отдавая предпочтение словам, **содержащим проблемные символы** из Парето-анализа. Ошибки концентрируются на том, что у вас не получается.

A new tutor mode `Error Practice` (button in the main window). It generates practice text from the **real language word dictionary** (e.g. `ru.words`), preferring words that **contain the problematic characters** from the Pareto analysis, so training focuses exactly on your weakest keys.

### 4. Исправление локализации / Locale fix

Добавлен вызов `setlocale()` — без него интерфейс всегда отображался на английском, даже при русской системной локали. Now the UI follows the system locale (e.g. Russian).

### 5. Исправление краха / Crash fix

Режим «Работа над ошибками» больше не падает при отсутствии данных об ошибках. Error Practice no longer crashes when there is no error data yet.

---

## / Как собрать и установить / Build & install

Зависимости (Arch Linux): `base-devel intltool gtk3 gtkdatabox curl autoconf automake libtool`

```bash
autoreconf -fi
./configure
make
sudo make install
```

Запуск: `klavaro`

---

## / Как пользоваться / Usage

1. **Тренируйтесь как обычно** — начните уроки в любом режиме: Базовый, Адаптируемость, Скорость, Беглость.
   **Practice as usual** in any mode: Basic, Adaptability, Velocity, Fluidity.
2. **Ошибки копятся автоматически** в `error_detail_<layout>.log`, Парето-анализ вычисляет топ проблемных символов.
   Errors accumulate automatically in `error_detail_<layout>.log`; the Pareto analysis computes the top problematic characters.
3. **Нажмите «Error Practice»** в главном окне — появится текст, насыщенный вашими «слабыми» буквами.
   Click **Error Practice** in the main window — a text saturated with your weak keys appears.
4. После нескольких сессий загляните в `pareto_analysis_<layout>.log`, чтобы видеть прогресс (какие символы перестали быть проблемными).
   After a few sessions, check `pareto_analysis_<layout>.log` to track which characters are no longer problematic.

Пути к логам: `~/.local/share/klavaro/` (наружу задаётся переменной `$XDG_DATA_HOME`).
Log locations: `~/.local/share/klavaro/` (overridden by `$XDG_DATA_HOME`).

---

## / Структура изменённых файлов / Changed files

| Файл / File | Изменение / Change |
|---|---|
| `src/error_logger.c/h` | **Новый** / **New** — логирование ошибок + Парето-анализ / error logging + Pareto analysis |
| `src/error_practice.c/h` | **Новый** / **New** — режим «Работа над ошибками» / Error Practice mode |
| `src/accuracy.c` | Вызовы логирования ошибок / error logging hooks |
| `src/tutor.c/h` | Новый тип урока `TT_ERROR_PRACTICE` / new tutor type |
| `src/callbacks.c/h` | Обработчик новой кнопки / new button handler |
| `src/main.c` | `setlocale()` — русский интерфейс / Russian UI, метки и кнопка / labels & button |
| `data/klavaro.glade` | Кнопка «Error Practice» в главном окне / button in main window |
| `src/Makefile.am` | Новые исходники в сборке / new sources in build |

---

## / Лицензия / License

**GNU General Public License v3.0** — см. [COPYING](COPYING). Основано на Klavaro 3.14 (C) Felipe Emmanuel Ferreira de Castro.

**GNU General Public License v3.0** — see [COPYING](COPYING). Based on Klavaro 3.14 (C) Felipe Emmanuel Ferreira de Castro.