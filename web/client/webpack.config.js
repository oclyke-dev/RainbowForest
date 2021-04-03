const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const CopyPlugin = require('copy-webpack-plugin');

module.exports = {
  entry: './src/index.tsx',
  output: {
    path: path.resolve(__dirname, 'dist'),
    filename: 'bundle.js',
  },
  resolve: {
    extensions: ['.tsx', '.ts', '.jsx', '.js', '.css', '.json'],
  },

  module: {
    rules: [
      {
        test: /\.(ts|tsx)$/,
        loader: 'ts-loader',
        include: path.resolve(__dirname, 'src'),
      },
      {
        test: /\.(js|jsx)$/,
        loader: 'babel-loader',
        include: path.resolve(__dirname, 'src'),
      },
      {
        test: /\.html$/,
        loader: 'html-loader',
        options: {
          sources: false, // Disables attributes processing so that copy plugin can provide favicon and base style sheet
        },
      },
      {
        test: /\.(mp3|png)$/,
        loader: 'file-loader',
      },
    ],
  },

  plugins: [
    new HtmlWebpackPlugin({
      template: 'src/index.html',
    }),
    new CopyPlugin({
      patterns: [
        { from: 'src/index.css', to: 'index.css' },
        { from: 'src/assets/ocl_L.png', to: 'assets/favicon.png' },
      ],
    }),
  ],

  devServer: {
    contentBase:  path.resolve(__dirname, 'dist'),
    port: 8080
  },
};
