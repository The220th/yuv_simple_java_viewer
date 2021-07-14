# yuv_simple_java_viewer

Штука, которая покажет изображение, сохранённое в yuv color space.

# Использование

Чтобы откомпилировать:

``` bash
> javac ViewerYUV.java
```

Пример запуска:

``` bash
> java ViewerYUV picture.yuv -w 640 -h 480 -f 420
```

где:

- picture.yuv - изображение, сохранённое в yuv color space

- -w 640 - ширина изображения

- -h 480 - высота изображения

- -f 420 - формат: 420 или 444. Реализованы только yuv420 и yuv444