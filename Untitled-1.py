#!/usr/bin/env python3.6
# _*_ coding:utf-8 _*_
__author__ = 'Chen Sy'
__version__ = '1.0'
__data__ = '2018, 12, 18'
__copyright__ = 'Copyright 2018, PI'
__all__ = []


from visiontool.core.common.datatype.collection import CollectionBase
from visiontool.tools.base import OperatorBase, ToolBase
from enum import Enum
import pandas as pd


class TableCollection(CollectionBase):
    def __init__(self):
        super(TableCollection, self).__init__()

    @property
    def rows(self):
        return self.len

    @property
    def cols(self):
        if self.rows == 0:
            return 0
        else:
            return self[0].len


class TableElem(pd.Series):
    def __init__(self, data=None):
        super(TableElem, self).__init__(data)

    @property
    def len(self):
        return self.__len__()


class DataTypeConstants(Enum):
    string = 0
    bool = 1
    numerical = 2


class CheckInput(object):
    @classmethod
    def checkIndex(cls, index, length):
        if index >= length:
            raise Exception("index out of range!")

    @classmethod
    def checkDataType(cls, item, data_type):
        if not isinstance(item, data_type):
            raise Exception("input type error!")


class SettingList(list):
    def __init__(self):
        super(SettingList, self).__init__()

    @property
    def len(self):
        return self.__len__()

    def add(self, item, index):
        pass

    def modify(self, item, index):
        pass

    def delete(self, index):
        pass

    def move(self, src_index, dst_index):
        pass


class NameSettingList(SettingList):
    def __init__(self):
        super(NameSettingList, self).__init__()

    def add(self, item, index):
        CheckInput.checkIndex(index, self.len + 1)
        CheckInput.checkDataType(item, str)
        if self.len != 0:
            self.insert(index + 1, item)
        else:
            self.insert(0, item)

    def modify(self, item, index):
        CheckInput.checkIndex(index, self.len)
        CheckInput.checkDataType(item, str)
        self[index] = item

    def delete(self, index):
        CheckInput.checkIndex(index, self.len)
        if index < len(self):
            del self[index]

    def move(self, src_index, dst_index):
        CheckInput.checkIndex(src_index, self.len)
        CheckInput.checkIndex(dst_index, self.len)
        if dst_index < src_index:
            temp = self[src_index]
            self.delete(src_index)
            self.insert(dst_index, temp)
        else:
            temp = self[src_index]
            self.insert(dst_index + 1, temp)
            self.delete(src_index)


class DataTypeSettingList(SettingList):
    def __init__(self):
        super(DataTypeSettingList, self).__init__()
        self.__data_type = []

    @property
    def data_type(self):
        return self.__data_type

    def __convertTypeEnumToType(self, data_type):
        operator_dic = {
            DataTypeConstants.string: str,
            DataTypeConstants.bool: bool,
            DataTypeConstants.numerical: float,
        }
        return operator_dic[data_type]

    def add(self, item, index):
        CheckInput.checkIndex(index, self.len + 1)
        CheckInput.checkDataType(item, DataTypeConstants)
        if self.len != 0:
            self.insert(index + 1, item)
            data_type = self.__convertTypeEnumToType(self[index + 1])
            self.__data_type.insert(index + 1, data_type)
        else:
            self.insert(0, item)
            data_type = self.__convertTypeEnumToType(self[0])
            self.__data_type.insert(0, data_type)

    def modify(self, item, index):
        CheckInput.checkIndex(index, self.len)
        CheckInput.checkDataType(item, DataTypeConstants)
        self[index] = item
        data_type = self.__convertTypeEnumToType(self[index])
        self.__data_type[index] = data_type

    def delete(self, index):
        CheckInput.checkIndex(index, self.len)
        if index < len(self):
            del self[index]
            del self.__data_type[index]

    def move(self, src_index, dst_index):
        CheckInput.checkIndex(src_index, self.len)
        CheckInput.checkIndex(dst_index, self.len)
        if dst_index < src_index:
            temp = self[src_index]
            del self[src_index]
            self.insert(dst_index, temp)
            temp = self.__data_type[src_index]
            del self.__data_type[src_index]
            self.__data_type.insert(dst_index, temp)
        else:
            temp = self[src_index]
            self.insert(dst_index + 1, temp)
            del self[src_index]
            temp = self.__data_type[src_index]
            self.__data_type.insert(dst_index + 1, temp)
            del self.__data_type[src_index]


class CreateData(OperatorBase):
    def __init__(self):
        super(CreateData, self).__init__()
        self.__table = TableCollection()
        self.__name_list = NameSettingList()
        self.__data_type_list = DataTypeSettingList()

    @property
    def name_list(self):
        return self.__name_list

    @property
    def data_type_list(self):
        return self.__data_type_list

    @property
    def table(self):
        return self.__table

    def setDataInTable(self, data, row_index, col_index):
        CheckInput.checkIndex(row_index, self.__table.rows)
        CheckInput.checkIndex(col_index, self.__table.cols)
        CheckInput.checkDataType(data, self.__data_type_list.data_type[col_index])
        self.__table[row_index][col_index] = data

    def addCols(self, col_index):
        CheckInput.checkIndex(col_index, self.__table.cols + 1)
        col_num = self.__table.cols
        for i in range(self.__table.rows):
            self.__table[i][col_num] = 0
            cols = list(self.__table[i].index)
            cols.insert(col_index + 1, cols.pop(cols.index(col_num)))
            self.__table[i] = TableElem(self.__table[i][cols])
            self.__table[i].index = range(self.__table.cols)

    def addRows(self, row_index):
        CheckInput.checkIndex(row_index, self.__table.rows + 1)
        table_elem = TableElem([])
        if self.__table.cols > 0:
            for i in range(self.__table.cols):
                table_elem[i] = None
        if self.__table.rows != 0:
            self.__table.insert(row_index + 1, table_elem)
        else:
            self.__table.insert(0, table_elem)

    def deleteCols(self, col_index):
        CheckInput.checkIndex(col_index, self.__table.cols)
        self.__data_type_list.delete(col_index)
        self.__name_list.delete(col_index)
        for i in range(self.__table.rows):
            self.__table[i].drop(index=col_index, inplace=True)
            self.__table[i].index = range(self.__table.cols)

    def deleteRows(self, row_index):
        CheckInput.checkIndex(row_index, self.__table.rows)
        self.__table.removeByIndex(row_index)

    def moveCols(self, src_col_index, dst_col_index):
        CheckInput.checkIndex(src_col_index, self.__table.cols)
        CheckInput.checkIndex(dst_col_index, self.__table.cols)
        self.__data_type_list.move(src_col_index, dst_col_index)
        self.__name_list.move(src_col_index, dst_col_index)
        for i in range(self.__table.rows):
            cols = list(self.__table[i].index)
            cols.insert(dst_col_index, cols.pop(cols.index(src_col_index)))
            self.__table[i] = TableElem(self.__table[i][cols])
            self.__table[i].index = range(self.__table.cols)

    def moveRows(self, src_row_index, dst_row_index):
        CheckInput.checkIndex(src_row_index, self.__table.cols)
        CheckInput.checkIndex(dst_row_index, self.__table.cols)
        if src_row_index > dst_row_index:
            table_elem = self.__table[src_row_index]
            self.deleteRows(src_row_index)
            self.__table.insert(dst_row_index, table_elem)

        else:
            table_elem = self.__table[src_row_index]
            self.__table.insert(dst_row_index + 1, table_elem)
            self.deleteRows(src_row_index)

    def modifyColsDataType(self, data_type, col_index):
        CheckInput.checkIndex(col_index, self.__table.cols)
        CheckInput.checkDataType(data_type, DataTypeConstants)
        if data_type != self.__data_type_list[col_index]:
            self.__data_type_list.modify(data_type, col_index)
            for i in range(self.__table.rows):
                self.__table[i][col_index] = None

    def modifyColsName(self, name, col_index):
        CheckInput.checkIndex(col_index, self.__table.cols)
        CheckInput.checkDataType(name, str)
        if name != self.__name_list[col_index]:
            self.__name_list.modify(name, col_index)
            for i in range(self.__table.rows):
                self.__table[i][col_index] = None


class CreateDataTool(ToolBase):
    def __init__(self, name):
        super(CreateDataTool, self).__init__(name=name)
        self._operator = CreateData()


if __name__ == "__main__":
    cst = CreateDataTool(name='cst')

    '''/**************************add & setData*******************************/'''
    cst.runParam.addRows(0)
    col_num0 = 0
    cst.runParam.data_type_list.add(DataTypeConstants.numerical, col_num0)
    cst.runParam.name_list.add('x', col_num0)
    cst.runParam.addCols(col_num0)
    cst.runParam.setDataInTable(1.0, 0, 0)

    col_num1 = 0
    cst.runParam.data_type_list.add(DataTypeConstants.numerical, col_num1)
    cst.runParam.name_list.add('y', col_num1)
    cst.runParam.addCols(col_num1)
    cst.runParam.setDataInTable(2.0, 0, 1)

    col_num2 = 1
    cst.runParam.data_type_list.add(DataTypeConstants.bool, col_num2)
    cst.runParam.name_list.add('flag', col_num2)
    cst.runParam.addCols(col_num2)
    cst.runParam.setDataInTable(True, 0, 2)

    cst.runParam.addRows(0)
    cst.runParam.setDataInTable(3.0, 1, 0)
    cst.runParam.setDataInTable(4.0, 1, 1)
    cst.runParam.setDataInTable(False, 1, 2)
    cst.runParam.addRows(1)

    print(cst.runParam.data_type_list.data_type)
    print(cst.runParam.name_list)
    print(cst.runParam.table[0])
    print(cst.runParam.table[1])
    print(cst.runParam.table[2])

    # cst.runParam.addRows(5)
    '''
    print('/***************************delete Cols****************************/')
    cst.runParam.deleteCols(2)
    print(cst.runParam.data_type_list.data_type)
    print(cst.runParam.name_list)
    print(cst.runParam.table[0])
    print(cst.runParam.table[1])
    print(cst.runParam.table[2])

    cst.runParam.deleteCols(1)
    print(cst.runParam.data_type_list.data_type)
    print(cst.runParam.name_list)
    print(cst.runParam.table[0])
    print(cst.runParam.table[1])
    print(cst.runParam.table[2])

    cst.runParam.deleteCols(0)
    print(cst.runParam.data_type_list.data_type)
    print(cst.runParam.name_list)
    print(cst.runParam.table[0])
    print(cst.runParam.table[1])
    print(cst.runParam.table[2])
    '''

    '''
    print('/***************************delete Rows****************************/')
    cst.runParam.deleteRows(2)
    print(cst.runParam.table)
    cst.runParam.deleteRows(1)
    print(cst.runParam.table)
    cst.runParam.deleteRows(0)
    print(cst.runParam.table)
    cst.runParam.deleteRows(0)
    print(cst.runParam.table)
    '''

    '''
    print('/***************************move Cols****************************/')
    cst.runParam.moveCols(3, 0)
    print(cst.runParam.data_type_list.data_type)
    print(cst.runParam.name_list)
    print(cst.runParam.table[0])
    print(cst.runParam.table[1])
    print(cst.runParam.table[2])
    '''

    '''
    print('/***************************move Rows****************************/')
    cst.runParam.moveRows(3, 1)
    print(cst.runParam.data_type_list.data_type)
    print(cst.runParam.name_list)
    print(cst.runParam.table[0])
    print(cst.runParam.table[1])
    print(cst.runParam.table[2])
    '''

    print('/***************************modify****************************/')
    cst.runParam.modifyColsDataType(DataTypeConstants.bool, 0)
    cst.runParam.modifyColsName('nothing', 0)
    print(cst.runParam.data_type_list.data_type)
    print(cst.runParam.name_list)
    print(cst.runParam.table[0])
    print(cst.runParam.table[1])
    print(cst.runParam.table[2])