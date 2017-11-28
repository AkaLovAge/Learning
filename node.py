class node:

	classification = None

	def __init__(self, is_leaf,value, split_index, parent, children, height):
		self.is_leaf = is_leaf
		self.split_key = split_index
		self.parent = parent
		self.children = children
		self.depth = height
		self.value = value

	def show(self):
		print self.is_leaf
		print self.split_key

class bi_node: 
	classification = None
	
	def  __init__(self,is_leaf, split_index, split_value, parent, left, right, height ):

		self.is_leaf = is_leaf
		self.split_key = split_index
		self.split_value = split_value
		self.parent = parent
		self.left = left
		self.right = right
		self.depth = height
	
