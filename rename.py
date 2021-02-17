# -*-coding=utf8-*-
import os

from pypinyin import lazy_pinyin


def rename():
    path = u"/Users/eggfly/Music/todo"
    file_list = os.listdir(path)  # 该文件夹下所有的文件（包括文件夹）
    for files in file_list:  # 遍历所有文件
        old_dir = os.path.join(path, files)  # 原来的文件路径
        if os.path.isdir(old_dir):  # 如果是文件夹则跳过
            continue
        filename = os.path.splitext(files)[0]  # 获取文件名
        # 把文件名里的汉字转换成其首字母
        filename1 = lazy_pinyin(filename)  # , style=pypinyin.FIRST_LETTER
        # 创建一个空列表
        filename2 = []
        for ch in filename1:
            filename2.extend(ch)
        # 把列表转换成没有间隔的字符串，因为文件名要以字符串形式存在
        filenameToStr = ''.join(filename2)
        filetype = os.path.splitext(files)[1]  # 文件扩展名
        Newdir = os.path.join(path, filenameToStr + filetype)  # 新的文件名
        os.rename(old_dir, Newdir)  # 重命名


rename()
