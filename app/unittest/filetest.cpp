#include "main.h"
#include <VFile>
#include <VDebugNew>

const static QString FILE_NAME = "abcd.txt";

TEST( File, createTest )
{
  VFile file(FILE_NAME, QIODevice::WriteOnly | QIODevice::Text);
  EXPECT_TRUE( file.open() );
  int writeLen = file.write("abcdefghijklmnopqrstuvwxyz");
  EXPECT_TRUE( writeLen == 26 );
  file.close();
}

TEST( File, readTest )
{
  VFile file(FILE_NAME, QIODevice::ReadOnly | QIODevice::Text);
  EXPECT_TRUE( file.open() );
  QFile& qfile = file;
  EXPECT_EQ( qfile.size(), 26 );

  char expect = 'a';
  int  readCnt = 0;
  for (int i = 0; i < 26; i++)
  {
    char real;
    int readLen = file.read(&real, 1);
    if (readLen == VERR_FAIL) break;
    EXPECT_EQ( expect, real );
    expect++;
    readCnt++;
  }
  EXPECT_EQ( readCnt, 26 );
  file.close();
}

TEST( File, appendTest )
{
  VFile file(FILE_NAME, QIODevice::Append | QIODevice::Text);
  EXPECT_TRUE( file.open() );
  int writeLen = file.write("ABCDEFGHIJKLMNOPQRSTUVEXYZ");
  EXPECT_TRUE( writeLen == 26 );
  file.close();

  file.mode = QIODevice::ReadOnly | QIODevice::Text;
  EXPECT_TRUE( file.open() );
  QFile& qfile = file;
  EXPECT_EQ( qfile.size(), 26 * 2 );
  file.close();
}
