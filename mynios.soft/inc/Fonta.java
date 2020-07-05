
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

   Font font = new Font(theme.getWindowTitleFont().getFamily(), Font.PLAIN, 30); // 创建字体

   AffineTransform at = new AffineTransform();

   FontRenderContext frc = new FontRenderContext(at, true, true);

   GlyphVector gv = font.createGlyphVector(frc, "ABC a你好  $@#*^"); // 要显示的文字

   Shape shape = gv.getOutline(5, 30); 

   int weith = 200; // 显示面板的宽

   int height = 40; // 显示面板的高

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