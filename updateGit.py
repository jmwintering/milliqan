import os

git_dir = "/Users/milliQan/OSUAnalysis/wintering/"
work_dir = "/Users/milliQan/work/"

print('\n')

# grab list of files from each directory + clean them
git_list = list()
for (dirpath, dirnames, filenames) in os.walk(git_dir):
	git_list += [os.path.join(dirpath, file) for file in filenames]

for i, path in enumerate(git_list):
	file = path[path.find("/wintering/")+9:len(path)]
	git_list[i] = file

work_list = list()
for (dirpath, dirnames, filenames) in os.walk(work_dir):
	work_list += [os.path.join(dirpath, file) for file in filenames]

for i, path in enumerate(work_list):
	file = path[path.find("/work/")+6:len(path)]
	work_list[i] = file


# clean up repo by removing trash
bad_files = ['.pdf', '.jpg', '.png', '.pyc', '.csv', '.json', '.root']

for file in git_list:
	for ext in bad_files:
		if ext in file: 
			try:
				os.system('rm -v '+git_dir+file)
			except:
				print('E: Error removing '+git_dir+file)
print('I: Git repo cleared of unwanted extensions!')



# clean up repo by removing elements not found in work dir
for file in git_list:
	if not file in work_list:
		try:
			os.system('rm -v '+git_dir+file)
		except:
			print('E: Error removing '+git_dir+file)
print('I: Git repo clear of non-local files!')



# now copy all relevant work files to git repo
allowed_files = ['.py', '.h', '.cpp', '.C', '.hpp', '.sh', '.nb']
exceptions = ['fitFunctions.root', 'fitFunctionsReadable.root']

for file in work_list:
	if os.path.islink(work_dir+file): continue

	good_file = False

	for ext in allowed_files:
		if ext in file:
			good_file = True

	for ext in bad_files:
		if ext in file:
			good_file = False

	for ext in exceptions: 
		if file == ext: good_file = True

	if good_file:
		os.system('cp -v '+work_dir+file+' '+git_dir+file)
print('I: Git repo synced!')

# now update git
git_message = input('A: Please enter a git commit message: ')
os.system('cd {} && git pull'.format(git_dir))

if git_message == '': git_message = 'Update'
os.system('cd {0} && git add -A && git commit -m \"{1}\" && git push'.format(git_dir, git_message))
	
print('I: All done! \n')