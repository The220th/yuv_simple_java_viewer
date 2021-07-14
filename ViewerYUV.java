import java.lang.*;
import java.util.*;
import java.io.*;
import java.awt.*;
import java.io.IOException;

import javax.swing.*;

public class ViewerYUV
{
    private static ViewerYUV picFrame;
    private static String pathToFile;
    private static int W;
    private static int H;
    private static String mode;
    private static int[] rgbImage;

    private JFrame mainFrame;
    private sComponent picom;

    public static void main(String[] args)
    {
        /*frags:
        first is pathToFile
        -w
        -h
        -f 420 or 444
        */
        if(args == null || args.length == 0)
        {
            System.out.println(help());
            System.exit(0);
        }

        pathToFile = args[0];
        for(int i = 1; i < args.length; ++i)
        {
            String s = args[i++];
            switch(s)
            {
                case "-w":
                {
                    W = Integer.valueOf(args[i]);
                    break;                    
                }
                case "-h":
                {
                    H = Integer.valueOf(args[i]);
                    break;
                }
                case "-f":
                {
                    mode = args[i];
                    if(!(mode.equals("420") || mode.equals("444")))
                    {
                        System.out.println(help());
                        System.exit(0);
                    }
                    break;
                }
                case "-?":
                {
                    System.out.println(help());
                    System.exit(0);
                    break;
                }
                default:
                {
                    System.out.println("Incorrect flags...");
                    System.out.println(help());
                    System.exit(0);
                    break;
                }
            }
        }

        int[] yuvImage = readImage();

        rgbImage = yuv2rgb(yuvImage, W, H);

        picFrame = new ViewerYUV();
        picFrame.show();
    }

    private static String help()
    {
        String res = "Syntax:";
        res += "\tThe first argument is the path to the image\n";
        res += "\t-w is width\n";
        res += "\t-h is height\n";
        res += "\t-f is format 420 or 444\n";
        res += "\t-? is help\n\n";

        res += "For example: \n";
        res += "> java ViewerYUV image.yuv -w 640 -h 480 -f 444\n\n";

        res += "Structure of the yuv-file if f=444:\n";
        res += "Suppose there is a 3x2 pixel image. That is, the width is 3 pixels, and the height is 2 pixels: \n";
        res += "For a pixel with coordinates x = 0, y = 0: y = y0, u = u0, v = v0.\n";
        res += "For a pixel with coordinates x = 1, y = 0: y = y1, u = u1, v = v1.\n";
        res += "For a pixel with coordinates x = 2, y = 0: y = y2, u = u2, v = v2.\n";
        res += "For a pixel with coordinates x = 0, y = 1: y = y3, u = u3, v = v3.\n";
        res += "For a pixel with coordinates x = 1, y = 1: y = y4, u = u4, v = v4.\n";
        res += "For a pixel with coordinates x = 2, y = 1: y = y5, u = u5, v = v5.\n";
        res += "Then the bytes in the file should go in the following order: y0, y1, y2, y3, y4, y5, u0, u1, u2, u3, u4, u5, v0, v1, v2, v3, v4, v5\n";
        return res;
    }
    
    private void show()
    {
        mainFrame = new JFrame("yuv image");
        //mainFrame.setSize(W, H);
        mainFrame.setLocation(50, 51);
        mainFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        picom = new sComponent(rgbImage, W, H);

        mainFrame.add(picom);

        mainFrame.pack();
        mainFrame.setVisible(true);
    }

    private static int[] readImage()
    {
        byte[] res = null;
        try(FileInputStream fin = new FileInputStream(pathToFile))
        {
            for(int frames_number = 0; frames_number < 1; ++frames_number)
            {
                if(mode.equals("444"))
                    res = new byte[W*H*3];
                else if(mode.equals("420"))
                    res = new byte[(W*H*3)/2];

                fin.read(res, 0, res.length);
            }
        }
        catch(IOException ex)
        {
            ex.printStackTrace();;
        }
        int[] resInt = new int[res.length];
        for(int li = 0; li < resInt.length; ++li)
            resInt[li] = unsignedByte(res[li]);
        if(mode.equals("420"))
            resInt = i420_to_i444(resInt, W, H);
        return resInt;
    }

    private static int[] i420_to_i444(int[] frame, int width, int height)
    {
        int[] planar = new int[W*H*3];
        int wh = width * height;
        int[] in = frame;

        int[] out = planar;
        int n, k, f, ni, nl, nin;
    
        /*for( n = 0 ; n < wh ; n ++ )
            ( *out ++ ) = ( *in ++ );*/
        for( n = 0 ; n < wh ; n++ )
            out[n] = in[n];
        
        nin = n;
    
        for( f = 0 ; f < 2 ; f ++ )
        {
            for( k = 0 ; k < height ; k ++ )
            {
                int[] line = in;
    
                for( ni = 0, nl = nin ; ni < width ; ni += 2)
                {
                    out[n] = line[nl];
                    n++;
                    out[n] = line[nl];
                    n++;
                    nl++;
                }
    
                if( k % 2 == 1 )
                    nin += width / 2;
            }
        }
        return out;
    }

    /**
     * Переводит byte в unsigned byte, но в виде int
     * 
     * @param x - byte, где восьмой бит единица
     * @return unsigned byte в виде int
     */
    public static int unsignedByte(byte x)
    {
        return (x&((byte)(-128)))==0?(int)x:(int)x+256;
    }

    private static int[] yuv2rgb(int[] yuv, int width, int height)
    {
        int[] tmp = new int[3*width*height];
        int red = 0;
        int green = width * height;
        int blue = width * height * 2;
         
        for(int k = 0; k < width*height; ++k)
        {
            /*
            //! Y
            tmp[k + red  ] =  0.299f   * img[k + red] + 0.587f   * img[k + green] + 0.114f   * img[k + blue];
            //! U
            tmp[k + green] = -0.14713f * img[k + red] - 0.28886f * img[k + green] + 0.436f   * img[k + blue];
            //! V
            tmp[k + blue ] =  0.615f   * img[k + red] - 0.51498f * img[k + green] - 0.10001f * img[k + blue];
            */
            /*
            //! Red   channel
            tmp[k + red  ] = img[k + red] + 1.13983f * img[k + blue];
            //! Green channel
            tmp[k + green] = img[k + red] - 0.39465f * img[k + green] - 0.5806f * img[k + blue];
            //! Blue  channel
            tmp[k + blue ] = img[k + red] + 2.03211f * img[k + green];
            */
            
            //! Red   channel
            tmp[k + red] = (int)(yuv[k + red] + 1.402f * (yuv[k + blue]-128)+0.5);
            //! Green channel
            tmp[k + green] = (int)(yuv[k + red] - 0.3441f * (yuv[k + green]-128) - 0.7141f * (yuv[k + blue]-128)+0.5);
            //! Blue  channel
            tmp[k + blue] = (int)(yuv[k + red] + 1.772f * (yuv[k + green]-128)+0.5);
            
            /*
            //! Red   channel
            tmp[k + red] = (int)(yuv[k + red] + 1.13983f * (yuv[k + blue]-128)+0.5);
            //! Green channel
            tmp[k + green] = (int)(yuv[k + red] - 0.39465f * (yuv[k + green]-128) - 0.58060f * (yuv[k + blue]-128)+0.5);
            //! Blue  channel
            tmp[k + blue] = (int)(yuv[k + red] + 2.03211f * (yuv[k + green]-128)+0.5);
            */

            if(tmp[k + red] > 255 || tmp[k + green] > 255 || tmp[k + blue] > 255
            || tmp[k + red] < 0 || tmp[k + green] < 0 || tmp[k + blue] < 0)
            {
                System.out.print("Problem with pixel " + k + ": ");
                System.out.print("yuv={" + yuv[k + red] + " " + yuv[k + green] + " " + yuv[k + blue] + "} <=> ");
                System.out.println("rgb={" + tmp[k + red] + " " + tmp[k + green] + " " + tmp[k + blue] + "}");
                tmp[k + red] = tmp[k + red]>255?255:tmp[k + red];
                tmp[k + green] = tmp[k + green]>255?255:tmp[k + green];
                tmp[k + blue] = tmp[k + blue]>255?255:tmp[k + blue];

                tmp[k + red] = tmp[k + red]<0?0:tmp[k + red];
                tmp[k + green] = tmp[k + green]<0?0:tmp[k + green];
                tmp[k + blue] = tmp[k + blue]<0?0:tmp[k + blue];
            }
        }
        return tmp;
    }

    private class sComponent extends JComponent
    {
    
        private int DEFAULT_WIDTH;
        private int DEFAULT_HEIGHT;
        private int[] pic;
        
        public sComponent(int[] pic, int W, int H)
        {
            this.pic = pic;
            DEFAULT_WIDTH = W;
            DEFAULT_HEIGHT = H;
        }
    
        public void paintComponent(Graphics gOld)
        {
            Graphics2D g = (Graphics2D)gOld;
            int w = DEFAULT_WIDTH;
            int h = DEFAULT_HEIGHT;
            int red = 0;
            int green = w * h;
            int blue = w * h * 2;
            Color c;
            int x, y;

            for(int i = 0; i < w*h; ++i)
            {
                x = i % w;
                y = i / w;
                c = new Color(pic[red+i], pic[green+i], pic[blue+i]);
                paintBlock(x, y, c, g);
            }
        }
    
        public Dimension getPreferredSize()
        {
            return new Dimension(DEFAULT_WIDTH, DEFAULT_HEIGHT);
        }
    
        private void paintBlock(int X, int Y, Color c, Graphics2D g)
        {
            //Color buff;
            //buff = g.getColor();
            g.setPaint(c);
            g.fillRect(X, Y, 1, 1);
            //g.setPaint(buff);
        }
    }
}
