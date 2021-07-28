import java.util.*;
import java.lang.*;
import java.io.*;
import java.awt.*;
import javax.swing.*;
import javax.swing.event.*;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;
import org.jcodec.api.FrameGrab;
import org.jcodec.api.JCodecException;
import org.jcodec.scale.*;
import org.jcodec.common.model.*;
import org.jcodec.common.io.*;

public class MP4toYUV
{
	private static String videoPath;
	private static String outputPath;

	private static File videoFile;

	private static JFrame mainFrame;
	private static sComponent picom;

	public static void main(String[] args)
	{
        if(args == null || args.length == 0|| args.length != 2)
        {
            System.out.println(help());
            System.exit(0);
        }
		videoPath = args[0];
		outputPath = args[1];

		videoFile = new File(videoPath);

		int i = 1;
		int totalFrames = getTotalFrames();
		boolean f = true;
		Picture picFrame;
		BufferedImage frame;
		picom = null;

		FileOutputStream fos = null;

        try
        {
        	fos = new FileOutputStream(outputPath);
        }
        catch(IOException ex)
        {
            ex.printStackTrace();
        }

		for(; i < totalFrames && f; ++i)
		{
			if(true /*Если нужны определенные кадры, то можно это указать явным образом
                     Например так:
                     70 <= i && i <= 225
			         || 295 <= i && i <= 475*/)
            {
    			/*Not even */try
    			{
    				picFrame = FrameGrab.getFrameFromFile(videoFile, i);
    				frame = AWTUtil.toBufferedImage(picFrame);
    				System.out.println(i + "/" + totalFrames);
    				if(mainFrame == null)
    				{
    					mainFrame = new JFrame("Test video");
    					picom = new sComponent(frame);
    					mainFrame.add(picom);
    					mainFrame.pack();
    			        mainFrame.setLocation(50, 51);
    			        mainFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    					mainFrame.show();
    				}
    				else
    					picom.update(frame);
    				//from 70 to 225
    				//from 295 to 475
    				//from 490 to 570
    				//from 635 to 660
    				int W = frame.getWidth();
    				int H = frame.getHeight();
    				int[] yuv420 = i444_to_i420(rgb2yuv(ImageToInts(frame), W, H), W, H);
    				saveBytes(yuv420, fos);
    			}
    			catch(Exception frameEnded)
    			{
    				//ignor
    				f = false;
    				frameEnded.printStackTrace();
    			}
            }
		}
		//fos.close();
	}

	private static int[] ImageToInts(BufferedImage pic)
	{
		int w = pic.getWidth();
		int h = pic.getHeight();
		int whc = w*h*3;
		int[] res = new int[whc];
        int red = 0;
        int green = w * h;
        int blue = w * h * 2;
		for(int i = 0; i < w*h; ++i)
		{
			int x = i % w;
			int y = i / w;
			int rgb = pic.getRGB(x, y);

			int r = (rgb >> 16) & 0xFF;
			int g = (rgb >>  8) & 0xFF;
			int b = (rgb      ) & 0xFF;

			res[red + i] = r;
			res[green + i] = g;
			res[blue + i] = b;
		}
		return res;
	}

	private static int[] rgb2yuv(int[] rgb, int width, int height)
    {
        int[] tmp = new int[3*width*height];
        int[] img = rgb;
        int red = 0;
        int green = width * height;
        int blue = width * height * 2;
         
        for(int k = 0; k < width*height; ++k)
        {
            //! Y
            tmp[k + red] = (int)(0.299f   * img[k + red] + 0.587f   * img[k + green] + 0.114f   * img[k + blue]+0.5);
            //! U
            tmp[k + green] = (int)(-0.1687f * img[k + red] - 0.3313f * img[k + green] + 0.5f   * img[k + blue] + 128+0.5);
            //! V
            tmp[k + blue] = (int)(0.5f   * img[k + red] - 0.4187f * img[k + green] - 0.0813f * img[k + blue]+128+0.5);

            if(tmp[k + red] > 255 || tmp[k + green] > 255 || tmp[k + blue] > 255
            || tmp[k + red] < 0 || tmp[k + green] < 0 || tmp[k + blue] < 0)
            {
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

    private static int[] i444_to_i420(int[] planar, int width, int height)
    {
		int wh = width * height;
		int[] in = planar;
		int[] out = new int[wh * 3 / 2];
		int n, k, f, gn, ln;

		for(int lli = 0; lli < out.length; ++lli)
			out[lli] = 0x80;

		for(gn = 0 ; gn < wh ; ++gn)
			out[gn] = in[gn];
		ln = gn;

		for(f = 0 ; f < 2 ; ++f)
		{
			for(k = 0 ; k < height ; k += 2)
			{
				for(n = 0 ; n < width ; n += 2)
				{
					out[gn] = in[ln];
					++gn;
					++ln;
					++ln;
				}
				ln += width;
			}
		}
		return out;
    }

	private static int getTotalFrames()
	{
		int res;
        try
        {
            res = FrameGrab.createFrameGrab(NIOUtils.readableChannel(new File(videoPath))).getVideoTrack().getMeta().getTotalFrames();
        }
        catch (Exception e)
        {
            throw new RuntimeException(e);
        }
        return res;
	}

	private static String help()
	{
        String res = "Syntax:";
        res += "\tThe first argument is the path to the video\n";
        res += "\tThe second argument is the path to the output yuv-video\n\n";

        res += "For example: \n";
        res += "> java MP4toYUV video.mp4 out.yuv\n\n";

        return res;
	}

    private static void saveBytes(int[] img, FileOutputStream fos)
    {
        byte[] res = new byte[img.length];
        for(int i = 0; i < res.length; ++i)
        	res[i] = signedByte(img[i]);
        try
        {
            fos.write(res, 0, res.length);
        }
        catch(IOException ex)
        {
            ex.printStackTrace();
        }
    }

    /**
     * Переводит unsigned byte, в виде int, в byte 
     * 
     * @param x - byte, где восьмой бит единица
     * @return byte со знаком
     * @throws IllegalArgumentException, если 0 > x или x >= 256
     */
    public static byte signedByte(int x) throws IllegalArgumentException
    {
        if(x < 0 || x >= 256)
            throw new IllegalArgumentException("Byte can be only [0; 255], you gave: " + x);
        return x < 128?(byte)x:(byte)(x-256);
    }

    private static class sComponent extends JComponent
    {
    
        private int DEFAULT_WIDTH;
        private int DEFAULT_HEIGHT;
        private BufferedImage pic;
        
        public sComponent(BufferedImage pic)
        {
            update(pic);
        }

        public void update(BufferedImage pic)
        {
            this.pic = pic;
            DEFAULT_WIDTH = pic.getWidth();
            DEFAULT_HEIGHT = pic.getHeight();
            SwingUtilities.updateComponentTreeUI(mainFrame);
            /*
			mainFrame.invalidate();
			mainFrame.validate();
			mainFrame.repaint();
            */
        }
    
        public void paintComponent(Graphics gOld)
        {
        	BufferedImage buffPic = pic;
            Graphics2D gMain = (Graphics2D)gOld;
            gMain.drawImage(buffPic, 0, 0, null);
        }
    
        public Dimension getPreferredSize()
        {
            return new Dimension(DEFAULT_WIDTH, DEFAULT_HEIGHT);
        }
    }
}