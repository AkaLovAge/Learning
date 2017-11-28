import sys
import collections 
from node import node,bi_node
import random

#import node.py 

def filereader(file_name):
	data=[]
	keys=[]
	with open(file_name) as f:
		for l in f:
			line = l.strip().split(" ")
			tmp = [x.split(':') for x in line]
			tmplist = collections.Counter()
			if (type(tmp[0][0]) is str):
				tmplist['label'] = tmp[0][0]
			else:
				print "the first element should be label"
				exit(1)

			for i in tmp[1:]:
				tmplist[i[0]] = i[1]
				keys.append(i[0])
			data.append(tmplist)
	return [data,list(set(keys))]

# Get the gini value 
#input:
#	groups:list for each group, each elem is [] for label  

def get_gini(groups):
	total_size = float(sum([len(x) for x in groups]))
	
	gini = 0

	for group in groups:
		size = float(len(group))
		if size == 0:
			continue
		count = collections.Counter(group)
		tsum = 0.0
		for key in count:
			tsum += (float(count[key])/size)*(float(count[key])/size)
		gini += (size/total_size)*(1-tsum)
	
	return gini

# get the combination of group base on value 

def get_com(labels):
	left = []
	right = []
	length = len(labels)
	labels_list = list(set(labels))
	if (length == 2):
		left.append([labels_list[0]])
		right.append([labels_list[1]])
		return {'left':left,'right':right}
	for i in range(1,length/2+1):
		j = 0
		com = [0]*i
		while (j>=0):
			com[j] += 1
			if (com[j]>length):
				j -= 1
			else:
				if (j== i-1):
					tmp = set([labels_list[index-1] for index in com])
					left.append(list(tmp))
					right.append(list(labels-tmp))
				else:
					j +=1
					com [j] = com[j-1]
	return {'left':left,'right':right}


#get split base on minimum gini value 

def get_split1(data_dic):
	data = data_dic[0]
	keys = data_dic[1]
	tmp_dic = collections.Counter()
	labels = [row['label'] for row in data]
	gini = 1
	split_return = {}
	
	for index in keys:
		split_attr = [row[index] for row in data]
		tmp_dic = get_com(set(split_attr))
		left = tmp_dic['left']
		right = tmp_dic['right']
		
		for i in range(0,len(left)):
			group1 =[]
			group2 =[]
			for j in range(0,len(split_attr)):
				if split_attr[j] in left[i]:
					group1.append(labels[j])
	
				if split_attr[j] in right[i]:
					group2.append(labels[j])

			groups = [group1,group2]
			new_gini = get_gini(groups)
			if gini > new_gini:
				gini = new_gini
				split_return = {'index':index,'value':{'left':left[i],'right':right[i]}}
	return split_return

#get split base on minimum gini value 

def get_split(data_dic):
	data = data_dic[0]
	keys = data_dic[1]
	gini = 1
	
	for index in keys:	
		split_attr = collections.Counter()
		for elem in data:
			if split_attr[elem['label']]:
				split_attr[elem['label']].append(elem[index])
			else:
				split_attr[elem['label']] = [elem[index]] 
		after_group = []
		for tmp_key in split_attr:
			after_group.append(split_attr[tmp_key])

		new_gini = get_gini(after_group)
		if gini > new_gini:
			gini = new_gini
			split_return = index
	return split_return

# return the most frequent class in node 

def to_determinate(data):
	group = [row['label'] for row in data]
	return max(set(group),key=group.count)

#split the node based on the split info 

def split1 (data,keys,n,depth):
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
		split_info = get_split1([left_data,keys])
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
		split_info = get_split1([right_data,keys])
		new_keys = [x for x in keys if x != split_info['index']]
		right_child = bi_node(False,split_info['index'], split_info['value'],n,None,None,depth+1)
		n.right = split1(right_data,new_keys,right_child,depth+1)

	return n

#split the node based on the split info 

def split (data,keys,n,depth):
	if not keys:
		n.classification = to_determinate(data)
		n.is_leaf = True
		print depth
		return n
	split_key = n.split_key 
	children_data = collections.Counter()

	for row in data:
		if children_data[row[split_key]]:
			children_data[row[split_key]].append(row)
		else:
			children_data[row[split_key]] = [row]

	children = []
	for branch in children_data:
		child = node(False,branch,None, n,[],depth+1)
		split_info = get_split([children_data[branch],keys])
		child_lab = list(set([row['label'] for row in children_data[branch]]))
		if (len(child_lab) == 1):
			child.classification = child_lab[0]
			child.is_leaf = True
		child.split_key = split_info
		child.height = depth+1
		new_keys = [i for i in keys if i != split_info]		
		children.append(split(children_data[branch],new_keys,child,depth+1))
		
	n.children = children
	print depth
	return n

#show the decision tree

def show_dTree(root,key):
	if (root.is_leaf):
		print '-'*root.depth,key,root.classification,root.is_leaf
		return 
	else:
		count = 0
		for child in root.children:
			count += 1
			print '-'*root.depth,key,root.split_key
			show_dTree(child,'node'+str(count))
		return
#show the decision tree 
def show_dTree1(root,key):
	if (root.is_leaf):
		print '-'*root.depth,key,root.classification,root.is_leaf
		return
	else:
		print '-'*root.depth,key,root.split_key
		show_dTree1(root.left,'left')
		show_dTree1(root.right,'right')
		return

#build up decision tree  
def decision_tree_builder1(data):
	split_info = get_split1(data)
	root = bi_node(False,split_info['index'],split_info['value'], None,None,None,0)
	keys = [i for i in data[1] if i != split_info['index']]
	return split1(data[0],keys,root,0)
#   show_dTree(root)



#build up decision tree 

def decision_tree_builder(data):
	split_info = get_split(data)
	root = node(False,None,split_info, None,[],0)
	keys = [i for i in data[1] if i != split_info]
	return split(data[0],keys,root,0)
#	show_dTree(root)

# use to predict a new dataset 

def predict1 (root, data_dic):
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
	
	for row in data:
		tmp_node = root
		counting =0
		while (not tmp_node.is_leaf):
			split_index = tmp_node.split_key
			split_value = tmp_node.split_value
			if (row[split_index] in split_value['left']):
				tmp_node = tmp_node.left
			else:
				if (row[split_index] in split_value['right']):
					tmp_node = tmp_node.right
				else:
					tmp_node=tmp_node.left
		clas = tmp_node.classification
		result[row['label']][clas] += 1
	return result
		
# use to predict a new dataset 

def predict (root, data_dic):
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
	
	for row in data:
		tmp_node = root
		counting =0
		while (not tmp_node.is_leaf):
			is_have = False
			for child in tmp_node.children:
				if (child.value == row[tmp_node.split_key]):
					tmp_node = child
					is_have = True 
					break
			if not is_have:
				tmp_node = random.choice(tmp_node.children)
				break
		clas = tmp_node.classification
		result[row['label']][clas] += 1
	return result

if __name__ == "__main__":
	train_file = sys.argv[1]
	test_file = sys.argv[2]
	
	train_data = filereader(train_file)
	test_data = filereader(test_file)

#	print ("training\n")
	#print train_data

#	print ("testing\n")
#	print test_data
#	labels = set([elem['1'] for elem in train_data])
	if len(train_data[0][0].keys()) > 10 :
		tree = decision_tree_builder1(train_data)
		result = predict1(tree,test_data)
		show_dTree1(tree,'root')
	else:
		tree = decision_tree_builder(train_data)
		show_dTree(tree,'root')
		result = predict(tree,test_data)
	print result 
	sum_top = 0
	total = 0
	for row in result:
		sum_top += result[row][row]
		for k in result[row]:
			total += result[row][k]

	print sum_top/(float)(total)
	#result = predict(tree,[[{'2': '5', 'label': '3', '1': '3', '4': '2', '6': '2', '8': '2', '3': '1', '5': '1', '7': '1'}],['1','2','3','4','5','6','7','8']])	

#	gini = get_gini([[1,1,1,1,1,1,1,2,2,2],[1,1,2,2]])
	

