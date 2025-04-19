use16
org 0x7C00

start:
    ; Инициализация сегментных регистров
    mov ax, cs
    mov ds, ax
    mov ss, ax
    mov sp, start

    ; Очистка экрана
    call clear_screen

    ; Вывод строки с выбором цвета
    mov si, color_prompt
    call print_string

    ; Чтение номера цвета с клавиатуры
    call read_char

    ; Сохранение номера цвета в памяти (по адресу 0x8E00)
    mov [0x8E00], al

    ; Загрузка ядра
    call load_kernel

    ; Переход в защищённый режим
    jmp switch_to_protected_mode

; Функция очистки экрана
clear_screen:
    mov ax, 0x03
    int 0x10
    ret

; Функция вывода строки (адрес строки в SI)
print_string:
    mov ah, 0x0E  ; Функция BIOS для вывода символа
.print_char:
    lodsb         ; Загрузка следующего символа из строки
    cmp al, 0     ; Проверка на конец строки
    je .done
    int 0x10      ; Вывод символа
    jmp .print_char
.done:
    ret

; Функция чтения символа с клавиатуры
read_char:
    mov ah, 0x00  ; Функция BIOS для чтения символа
    int 0x16      ; Ожидание ввода
    ret

; Функция загрузки ядра
load_kernel:
    mov bx, 0x1000  ; Загрузка по адресу 0x1000:0x0000
    mov es, bx
    xor bx, bx
    mov dl, 0x01    ; Диск B
    mov dh, 0x00    ; Головка
    mov ch, 0x00    ; Цилиндр
    mov cl, 0x01    ; Сектор
    mov al, 0x60    ; Количество секторов для загрузки
    mov ah, 0x02    ; Функция чтения диска
    int 0x13
    jc disk_error   ; Если произошла ошибка, перейти к обработке ошибки
    ret

; Обработка ошибки диска
disk_error:
    mov si, disk_error_msg
    call print_string
    hlt

; Переход в защищённый режим
switch_to_protected_mode:
    cli             ; Отключение прерываний
    lgdt [gdt_info] ; Загрузка GDT

    ; Включение адресной линии A20
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Переход в защищённый режим
    mov eax, cr0
    or al, 1
    mov cr0, eax

    ; Дальний переход для загрузки корректной информации в CS
    jmp 0x8:protected_mode

use32
protected_mode:
    ; Загрузка селекторов сегментов
    mov ax, 0x10
    mov es, ax
    mov ds, ax
    mov ss, ax

    ; Передача управления ядру
    call 0x10000

; Данные
color_prompt:
    db "Press: 1-green,2-blue,3-red,4-yellow,5-gray,6-white:", 0

disk_error_msg:
    db "Disk error!", 0

; GDT
gdt:
    ; Нулевой дескриптор
    db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    ; Сегмент кода
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00
    ; Сегмент данных
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00

gdt_info:
    dw gdt_info - gdt - 1  ; Лимит GDT
    dd gdt                 ; Адрес GDT

; Заполнение оставшейся части загрузочного сектора
times 510-($-$$) db 0
dw 0xAA55  ; Сигнатура загрузочного сектора
