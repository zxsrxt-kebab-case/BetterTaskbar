
<p align="center">
  <img src="pics/BetterTaskBarLogo.png" alt="BetterTaskbar Logo" width="300">
</p>

# BetterTaskbar

A custom taskbar controller for Windows written in C++20. It replaces the default, often clunky auto-hide behavior with smooth, customizable animations and better focus management.

## Features

* **Smooth Animations:** Replaces instant snapping with calculated frame-by-frame animations. Supports multiple easing functions (Linear, Ease-Out, Ease-In-Out, Bounce).
* **System Tray Control:** Manage the taskbar directly from the system tray. Change animation styles, adjust speed, or toggle the script on/off on the fly.
* **Smart Start Menu Detection:** Fixes the default Windows key bug. The taskbar stays visible as long as the Start Menu or Windows Search is open.
* **State Synchronization:** Prevents Windows from randomly forcing the taskbar to appear when minimizing full-screen applications.

## Usage

1. **Important:** Disable the default **"Automatically hide the taskbar in desktop mode"** in your Windows settings.
2. Run `taskbar.exe`.
3. Right-click the new icon in your system tray to configure your preferred animation speed and style, or to exit the application gracefully.
4. *(Optional)* Add the executable to your Windows Startup folder to run it automatically on boot.

## Building from Source

This project uses **C++20** features (specifically `std::lerp` and `std::chrono`).

* Build system: **CMake**
* Toolchain: Works perfectly with **MSVC** (Visual Studio) or modern **MinGW**.
* If building in CLion or Visual Studio, ensure the project is configured as a `WINDOWS` subsystem to hide the console window on startup.

---

# BetterTaskbar (RU)

Кастомный контроллер панели задач для Windows на C++20. Заменяет стандартное, зачастую дерганое автоматическое скрытие на плавные анимации и решает проблемы с фокусом.

## Особенности

* **Плавные анимации:** Вместо мгновенного телепортирования панель выезжает плавно. Поддерживаются разные стили (Linear, Ease-Out, Ease-In-Out, Bounce).
* **Управление через трей:** Настройка скорости и стиля анимации на лету через контекстное меню в системном трее.
* **Умная обработка меню "Пуск":** Панель не прячется, пока вы пользуетесь поиском или меню "Пуск" (фикс бага с кнопкой Win).
* **Принудительная синхронизация стейта:** Не дает Windows самовольно возвращать панель на экран при сворачивании активных окон.

## Как использовать

1. **Важно:** Выключите стандартную опцию **"Автоматически скрывать панель задач"** в настройках Windows.
2. Запустите `taskbar.exe`.
3. Нажмите правой кнопкой мыши по иконке в трее, чтобы выбрать скорость, стиль анимации или закрыть программу (панель вернется в стандартное состояние).
4. *(Опционально)* Добавьте программу в автозагрузку Windows.

## Сборка

Проект использует возможности стандарта **C++20** (`std::lerp`, `std::chrono`).

* Система сборки: **CMake**
* Компилятор: **MSVC** (Visual Studio) или свежий **MinGW**.
* При сборке через CLion или Visual Studio убедитесь, что подсистема установлена как `WINDOWS`, чтобы избежать появления черного окна консоли при запуске.

---
