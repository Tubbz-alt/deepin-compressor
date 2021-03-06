/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     gaoxiang <gaoxiang@uniontech.com>
*
* Maintainer: gaoxiang <gaoxiang@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dmimedatabase.h"

#include <gtest/gtest.h>

#include <QFileInfo>

class TestDMimeDatabase : public ::testing::Test
{
public:
    TestDMimeDatabase() {}

public:
    virtual void SetUp()
    {
        m_tester = new DMimeDatabase;
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    DMimeDatabase *m_tester;
};

TEST_F(TestDMimeDatabase, initTest)
{
}

TEST_F(TestDMimeDatabase, testmimeTypeForFile001)
{
    QString strFileName = "../test/UnitTest/regress/new.zip";
    m_tester->mimeTypeForFile(strFileName);
}

TEST_F(TestDMimeDatabase, testmimeTypeForFile002)
{
    QString strFileName = "../test/UnitTest/regress/new.zip";
    m_tester->mimeTypeForFile(QFileInfo(strFileName));
}
