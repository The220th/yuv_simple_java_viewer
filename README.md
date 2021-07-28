# yuv_simple_java_viewer

Штука, которая покажет видео (изображение), сохранённое в yuv color space. Если видео, то показ будет происходить покадрово.

# Использование

## Компиляция

Чтобы откомпилировать:

``` bash
> javac ViewerYUV.java
```

## Пример запуска:

``` bash
> java ViewerYUV video.yuv -w 640 -h 480 -f 420 -d video2.yuv
```

где:

- `video.yuv` - видео (изображение), сохранённое в yuv color space

- `-w 640` - ширина изображения

- `-h 480` - высота изображения

- `-f 420` - формат: 420 или 444. Реализованы только yuv420 и yuv444

- `-d video2.yuv` - второе видео (изображение). Это *опциональный* параметр. Это видео может использоваться, чтобы сравнивать с исходным. У `video2.yuv` должно быть такие же ширина и высота, как и у `video.yuv`. Количество кадров у `video2.yuv` должно быть не меньше, чем у `video.yuv`.

Если неправильно ввести ширину, высоту или формат (или не ввести вовсе), то работа программы будет непредсказуема.

# Дополнительные программы

В директории `utils`:

- [AddNoise](https://github.com/The220th/yuv_simple_java_viewer) добавляет шум на yuv изображение.