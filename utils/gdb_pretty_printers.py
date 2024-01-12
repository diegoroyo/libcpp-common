import gdb.printing


class VecPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        T = str(self.val.type.template_argument(0))
        N = int(self.val.type.template_argument(1))
        T_map = {
            'float': 'f',
            'unsigned int': 'u',
            'int': 'i',
        }
        format_map = {
            'float': lambda x: '{:.3f}'.format(float(x)) if (float(x) == 0) or (0.001 <= abs(float(x)) < 1000) else '{:.3e}'.format(float(x)),
            'unsigned int': lambda x: '{:d}'.format(int(x)) if int(x) < 1000 else '{:e}'.format(int(x)),
            'int': lambda x: '{:d}'.format(int(x)) if abs(int(x)) < 1000 else '{:e}'.format(int(x)),
        }
        if 2 <= int(N) <= 4 and T in T_map.keys():
            name = f'Vec{N}{T_map[T]}'
        else:
            name = f'Vec<{T}, {N}>'
        name += '(' + ', '.join([format_map[T](self.val['_M_elems'][i])
                                 for i in range(N)]) + ')'
        return name

    # def children(self):
    #     size = int(self.val.type.template_argument(1))
    #     if 2 <= size <= 4:
    #         return

    #     # Yield the elements as children
    #     for i in range(size):
    #         if 2 <= size <= 4:
    #             yield ('[{}]'.format('xyzw'[i]), self.val['_M_elems'][i])
    #         else:
    #             yield ('[{}]'.format(i), self.val['_M_elems'][i])


def build_pretty_printer_vec():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("Vec")
    pp.add_printer('Vec', '^Vec<.*>$', VecPrinter)
    return pp


class ColorPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        T = str(self.val.type.template_argument(0))
        N = int(self.val.type.template_argument(1))
        T_map = {
            'float': 'f',
            'unsigned int': 'u',
            'int': 'i',
        }
        format_map = {
            'float': lambda x: '{:.3f}'.format(float(x)) if (float(x) == 0) or (0.001 <= abs(float(x)) < 1000) else '{:.3e}'.format(float(x)),
            'unsigned int': lambda x: '{:d}'.format(int(x)) if int(x) < 1000 else '{:e}'.format(int(x)),
            'int': lambda x: '{:d}'.format(int(x)) if abs(int(x)) < 1000 else '{:e}'.format(int(x)),
        }
        if 2 <= int(N) <= 4 and T in T_map.keys():
            name = f'Color{N}{T_map[T]}'
        else:
            name = f'Color<{T}, {N}>'
        name += '(' + ', '.join([format_map[T](self.val['_M_elems'][i])
                                 for i in range(N)]) + ')'
        return name

    # def children(self):
    #     size = int(self.val.type.template_argument(1))
    #     if 2 <= size <= 4:
    #         return

    #     # Yield the elements as children
    #     for i in range(size):
    #         if 2 <= size <= 4:
    #             yield ('[{}]'.format('rgba'[i]), self.val['_M_elems'][i])
    #         else:
    #             yield ('[{}]'.format(i), self.val['_M_elems'][i])


def build_pretty_printer_color():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("Color")
    pp.add_printer('Color', '^Color<.*>$', ColorPrinter)
    return pp


gdb.printing.register_pretty_printer(None, build_pretty_printer_vec())
gdb.printing.register_pretty_printer(None, build_pretty_printer_color())
