import sys 
import collections 
from node import node, bi_node
from DecisionTree import *
import random 

forest_size = 100
#split the node based on the split info 

def random_split (data,keys,n,depth):
	if not keys:
		n.is_leaf = True
		return n
	left_info = n.split_value['left']
	right_info = n.split_value['right']
	split_key = n.split_key 
	left_data = [row for row in data if row[split_key] in left_info]
	right_data = [row for row in data if row[split_key] in right_info]
	if left_data:
		left_label = set([row['label'] for row in left_data])
	if right_data:
		right_label = set([row['label'] for row in right_data])
	
	if ((not left_data) or (not right_data)):
		n.split_key = None
		n.split_value = None
		n.is_leaf = True		
		n.classification = to_determinate(left_data + right_data)
		return n
	
	if (len(left_label) == 1 or len(keys)  == 1):
		left_child = bi_node(True,None,None, n,None,None,depth+1)
		if len(left_label) == 1:
			left_child.classification = list(left_label)[0]
		if len(keys) == 1:
			left_child.classification = to_determinate(left_data)
		n.left = left_child

	else:
		
		random.shuffle(keys)
		random_keys = keys[0:(int(len(keys)*0.6+1))]

		split_info = get_split1([left_data,random_keys])
		new_keys = [x for x in keys if x != split_info['index']]
		left_child = bi_node(False,split_info['index'], split_info['value'],n,None,None,depth+1)
		n.left = split1(left_data,new_keys,left_child,depth+1)
			

	if (len(right_label) == 1 or len(keys)  == 1):
		right_child = bi_node(True,None,None, n,None,None,depth+1)
		if len(right_label) == 1:
			right_child.classification = list(right_label)[0]
		if len(keys) == 1:
			right_child.classification = to_determinate(right_data)
		n.right = right_child
	else:
		random.shuffle(keys)
		random_keys = keys[0:int(len(keys)*0.6+1)]
		split_info = get_split1([right_data,keys])
		new_keys = [x for x in keys if x != split_info['index']]
		right_child = bi_node(False,split_info['index'], split_info['value'],n,None,None,depth+1)
		n.right = split1(right_data,new_keys,right_child,depth+1)

	return n



def forest_builder(data_dic):
	keys = data_dic[1]
	random.shuffle(keys)
	random_key = keys[0:int(len(keys)*0.6+1)]
	split_info = get_split1([data_dic[0],random_key])
	root = bi_node(False,split_info['index'], split_info['value'], None,None,None,0)
	keys = [i for i in data_dic[1] if i != split_info['index']]
	return random_split(data_dic[0],keys,root,0)
	
def forest_predict (forest, data_dic):
	data = data_dic[0]
	keys = data_dic[1]
	labels = [row['label'] for row in data]
	label =list(set(labels))
	l = len(label)
	result = collections.Counter()
	for i in range(0,l):
		sub_dic = collections.Counter()
		for j in range(0,l):
			sub_dic[label[j]]=0
			result[label[i]] = sub_dic
	count =0
	for row in data:
		count += 1
		clas = []
		for one_tree in forest: 			
			tmp_node = one_tree 
			while (not tmp_node.is_leaf):
				split_index = tmp_node.split_key
				split_value = tmp_node.split_value
				if (row[split_index] in split_value['left']):
					tmp_node = tmp_node.left
				else:
					tmp_node = tmp_node.right
			clas.append(tmp_node.classification)
		final_clas = max(set(clas),key=clas.count)
		result[row['label']][final_clas] += 1
	return result

if __name__ == '__main__':
	
	train_file = sys.argv[1]
	test_file = sys.argv[2]

	train_data = filereader(train_file)
	test_data = filereader(test_file)

	
	forest = []
	count = 0
	for i in range(0,forest_size):
		print count,"tree"
		count +=1
		forest.append(forest_builder(train_data))
#		show_dTree(forest[i],'root')
	
	result = forest_predict(forest,test_data)
	sum_top = 0
	total = 0
	for row in result:
		sum_top += result[row][row]
		for k in result[row]:
			total += result[row][k]
	print sum_top/(float)(total)

	print result 
