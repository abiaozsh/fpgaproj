
import java.awt.Font;

import java.awt.Shape;

import java.awt.font.FontRenderContext;

import java.awt.font.GlyphVector;

import java.awt.geom.AffineTransform;



import javax.swing.plaf.metal.DefaultMetalTheme;



public class Fonta {



 public static void main(String[] args) {

  try {

   DefaultMetalTheme theme = new DefaultMetalTheme();

   Font font = new Font(theme.getWindowTitleFont().getFamily(), Font.PLAIN, 30); // ��������

   AffineTransform at = new AffineTransform();

   FontRenderContext frc = new FontRenderContext(at, true, true);

   GlyphVector gv = font.createGlyphVector(frc, "ABC a���  $@#*^"); // Ҫ��ʾ������

   Shape shape = gv.getOutline(5, 30); 

   int weith = 200; // ��ʾ���Ŀ�

   int height = 40; // ��ʾ���ĸ�

   boolean[][] view = new boolean[weith][height];

   for (int i = 0; i < weith; i++) {

    for (int j = 0; j < height; j++) {

     if (shape.contains(i, j)) {

      view[i][j] = true;

     } else {

      view[i][j] = false;

     }

    }

   }

   for (int j = 0; j < height; j++) {

    for (int i = 0; i < weith; i++) {

     if (view[i][j]) {

      System.out.print("*");

     } else {

      System.out.print(" ");

     }

    }

    System.out.println();

   }

  } catch (Exception e) {

   e.printStackTrace();

  }

 }

}