class Node:
    def __init__(self):
        self.list_itemset = []
        self.support = 0
        self.children = []

    def __str__(self):
        return "(" + str(self.list_itemset).replace("[", "").replace("]", "").replace(" ", "") + " : " + str(self.support) + ")"

    def __eq__(self, __value: object) -> bool:
        return self.list_itemset == __value.get_list_itemset()
    
    def __hash__(self):
        return hash(str(self))
    
    def add_list_itemset(self, list_itemset):
        self.list_itemset.append(list_itemset)
        self.list_itemset.sort()

    def add_child(self, obj):
        self.children.append(obj)

    def add_support(self, support):
        self.support += support

    def get_children(self):
        return self.children

    def get_list_itemset(self):
        return self.list_itemset

    def get_support(self):
        return self.support

    def set_children(self, children):
        self.children = children

    def set_list_itemset(self, list_itemset):
        self.list_itemset = list_itemset
        self.list_itemset.sort()

    def set_support(self, support):
        self.support = support

class Forest:
    def __init__(self):
        self.sources = []

    def __str__(self):
        return str(self.sources)

    def get_sources(self):
        return self.sources
    
    def set_sources(self, sources):
        self.sources = sources

    def add_source(self, obj):
        if obj not in self.sources:
            self.sources.append(obj)

    def add_node(self, node):
        def recc_add_node(node, sources):
            if not node in sources:
                sources.append(node)
            else:
                sources[sources.index(node)].add_support(node.get_support())
                for child in node.get_children():
                    recc_add_node(child, sources[sources.index(node)].get_children())

        recc_add_node(node, self.sources)

    def __str__(self) -> str:
        def list_node_from_source_recc(source, level):
            res = ""
            for i in range(level):
                res += "   "
            res += str(source) + "\n"
            for node in source.get_children():
                res += list_node_from_source_recc(node, level + 1)
            return res
            
        str_res = ""
        for source in self.sources:
            str_res += list_node_from_source_recc(source, 0)
        
        return str_res
    

class OutputParser:
    def __init__(self, output):
        self.output = output
        self.patterns = Forest()
        self.parse()

    def parse(self):
        with open(self.output, 'r') as f:
            for line in f:   
                if line.__contains__("#SUP:"):
                    tabLine = line.split("-1")
                    for i in range(len(tabLine)):
                        tabLine[i] = tabLine[i].rstrip()
                        tabLine[i] = tabLine[i].lstrip()
                        tabLine[i] = tabLine[i].replace("<", "")
                        tabLine[i] = tabLine[i].replace(">", "")
                        tabLine[i] = tabLine[i].split(" ")

                    #Maintenant, le la première colonne est le temps relatif
                    #De la deuxième colonne à la dernière, on a les items

                    node_line = None
                    temp_node = None
                    for i in range(len(tabLine)-1):
                        node = Node()
                        itemset = []
                        for j in range(1, len(tabLine[i])):
                            itemset.append({int(tabLine[i][j])})
                        node.set_list_itemset(itemset)
                        node.set_support(int(tabLine[len(tabLine)-1][1]))

                        if i == 0:
                            node_line = node
                            temp_node = node
                        else:
                            temp_node.add_child(node)
                            temp_node = node

                    self.patterns.add_node(node_line)


if __name__ == "__main__":
    output = "output.txt"
    parser = OutputParser(output)
    parser.parse()
