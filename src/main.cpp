﻿#include <iostream>
#include <exception>

#include "lib_version.h"
#include "CLParser.h"
#include "matrix.h"

using namespace std;
using namespace roro_lib;

void help()
{
      cout << R"(
 This demo is my matrix.

    matrix  [-version | -? | -math_oder_dimensions]
       Options:  
       -version                -get version of program
       -?                      -about program (this info)
       -math_oder_dimensions   -output dimensions at math order (like x, y). Example: column, row   
                                By Default: row, column
)" << endl;
}

void version_matrix()
{
      cout << "Version matrix: " << version() << endl;
}



#ifndef _TEST

int main(int argc, char* argv[])
{
      try
      {
            ParserCommandLine PCL;
            PCL.AddFormatOfArg("?", no_argument, '?');
            PCL.AddFormatOfArg("help", no_argument, '?');
            PCL.AddFormatOfArg("version", no_argument, 'v');
            PCL.AddFormatOfArg("math_oder_dimensions", no_argument, 'm');

            PCL.SetShowError(false);
            PCL.Parser(argc, argv);

            if (PCL.Option['?'])
            {
                  help();
                  return 0;
            }
            if (PCL.Option['v'])
            {
                  version_matrix();
                  return 0;
            }

            matrix<int, 0> diagonal_matrix;

            for (int i = 0; i < 10; ++i)
            {
                  diagonal_matrix[i][i] = i;
            }

            for (int i = 0; i < 10; ++i)
            {
                  diagonal_matrix[i][9 - i] = 9 - i;
            }

            for (int i = 1; i < 9; ++i)
            {
                  for (int j = 1; j < 9; ++j)
                  {
                        cout << diagonal_matrix[i][j] << " ";
                  }
                  cout << "\n";
            }

            cout << diagonal_matrix.size() << "\n";

            for (auto const& [row, column, v] : diagonal_matrix)
            {
                  if (PCL.Option['m'])
                  {
                        std::cout << column << "x" << row << " " << v << std::endl;
                  }
                  else
                  {
                        std::cout << row << "x" << column << " " << v << std::endl;
                  }                  
            }
      }
      catch (const exception& ex)
      {
            cerr << "Error: " << ex.what() << endl;
            return EXIT_FAILURE;
      }
      catch (...)
      {
            cerr << "Error: unknown exception" << endl;
            return EXIT_FAILURE;
      }

      return EXIT_SUCCESS;
}

#endif
