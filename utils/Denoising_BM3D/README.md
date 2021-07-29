# Denoising_BM3D

Штука, которая уберёт гауссовский шум из видео (изображения), сохранённое в yuv color space.

Используется алгоритм BM3D. Код `./docs/bm3d_src.tar.gz` взят [отсюда](https://www.ipol.im/pub/art/2012/l-bm3d/). Также существует [Python версия](https://github.com/Ryanshuai/BM3D_py).

Материалы по теме.

- [./docs/BM3D_TIP_2007.pdf](https://webpages.tuni.fi/foi/GCF-BM3D/BM3D_TIP_2007.pdf)

- [./docs/article_lr.pdf](https://www.ipol.im/pub/art/2012/l-bm3d/article_lr.pdf)

- [./docs/ICIP2019_Ymir.pdf](https://webpages.tuni.fi/foi/papers/ICIP2019_Ymir.pdf)

- [./docs/SPARS09-BM3D-SAPCA.pdf](https://webpages.tuni.fi/foi/papers/SPARS09-BM3D-SAPCA.pdf)

# Использование

## Зависимости

Для компиляции и работы программы необходима библиотека [fftw3](https://www.fftw.org/). Например, в `Ubuntu 20.04` установить библиотеку можно следующей командой:

``` bash
> sudo apt install libfftw3-dev
```

Также есть сами файлы библиотеки в директории `./lib`.

## Компиляция

``` bash
> make
```

## Пример запуска:

Программа принимает 5 аргументов. Если ввести аргументов не 5 или с ошибкой, то поведение непредсказуемо.

``` bash
> ./AddNoise video.yuv 640 480 243 10
```

где:

- Аргумент 1: `video.yuv` - видео (изображение), сохранённое в yuv color space

- Аргумент 2: `640` - ширина изображения

- Аргумент 3: `480` - высота изображения

- Аргумент 4: `243` - количество кадров в `video.yuv`

- Аргумент 5: `10` - сигма, число от 1 до 70 (но можно, в теории, поставить значение больше). Чем выше значение, тем изначально "шумнее" видео.

В рабочей директории появится файл `result.yuv`.

## Дополнение

Чтобы изменить настройки "денойзинга", можно поменять вызов функции `bm3dAdapter` в функции `main`.