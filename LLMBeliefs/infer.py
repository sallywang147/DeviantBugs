import os
import openai
from openai import OpenAI

RESULTS_DIR = "../LLMResults"

def gpt_infer(prompt):
	client = OpenAI(api_key=os.environ.get("OPENAI_API_KEY"))
	chat_completion = client.chat.completions.create(
			messages=[
					{
					"role": "user",
					"content": f"{prompt}",
					}
			],
			model="gpt-4o",
	)
	return chat_completion.choices[0].message.content

def infer_beliefs(patchFile, bugFile, bugFixFile):
	patchread = open(patchFile, "r")
	patch = patchread.read()
	bugread = open(bugFile, "r")
	bug = bugread.read()
	bug_fix_read = open(bugFixFile, "r")
	bug_fix = bug_fix_read.read()
	patch_prompt = f"""
	Your task is to understand code and infer two sets of programmers' beliefs from three related programs: a full patch on a buggy program, \
	a (sometimes partial) buggy program, and the fixed program. For each given program, you should infer two sets of beliefs:\
		i) MUST beliefs, defined as  directly implied by the code and there is no doubt that the programmer has that belief. \
	For example, a pointer dereference implies that a programmer must believe the pointer is non-null (assuming they want safe\
       code). ii) MAY beliefs, defined as code features that suggest a belief but may instead be a coincidence. For example, \
	       a call to "a" followed by a call to "b" implies the programmer may believe they must be paired, but it could be a coincidence. \
		       Plese carefully study the patch below and generate a list of MUST beliefs and MAY beliefs by the programmer regarding \
	this patch: {patch}. In your inferred beliefs, please also point to specific code to identify where the inferred beliefs are based on. 
	"""
	bug_prompt = f"""
	Your task is to understand code and infer two sets of programmers' beliefs from three related programs: a full patch on a buggy program, \
	a (sometimes partial) buggy program, and the fixed program. For each given program, you should infer two sets of beliefs:\
		i) MUST beliefs, defined as  directly implied by the code and there is no doubt that the programmer has that belief. \
	For example, a pointer dereference implies that a programmer must believe the pointer is non-null (assuming they want safe\
	code). ii) MAY beliefs, defined as code features that suggest a belief but may instead be a coincidence. For example, \
		a call to "a" followed by a call to "b" implies the programmer may believe they must be paired, but it could be a coincidence. \
		Please carefully study the original bug snippets from the prior patch and infer a list of MUST beliefs \
	and MAY beliefs held by the programmer regarding the buggy code snippet: {bug}. In your inferred beliefs, please also point to specific code to identify where the inferred beliefs are based on.
	"""
	bugfix_prompt = f"""
	Your task is to understand code and infer two sets of programmers' beliefs from three related programs: a full patch on a buggy program, \
	a (sometimes partial) buggy program, and the fixed program. For each given program, you should infer two sets of beliefs:\
		i) MUST beliefs, defined as  directly implied by the code and there is no doubt that the programmer has that belief. \
	For example, a pointer dereference implies that a programmer must believe the pointer is non-null (assuming they want safe\
       code). ii) MAY beliefs, defined as code features that suggest a belief but may instead be a coincidence. For example, \
	       a call to "a" followed by a call to "b" implies the programmer may believe they must be paired, but it could be a coincidence. \
       Please carefully study the fixed code snippets from the prior patch and \
	       infer a list of MUST beliefs and MAY beliefs held by the programmer regarding the corrected code snippet: {bug_fix}. In your inferred beliefs, please also point to specific code to identify where the inferred beliefs are based on.
	"""
	patch_out = gpt_infer(patch_prompt)
	print(f"BELIEFS ABOUT PATCHS:\n {patch_out}")
	fullpatchname = os.path.basename(patchFile)
	patch_name, _ = fullpatchname.split(".")
	patchwrite = open(f"{RESULTS_DIR}/{patch_name}.txt", "w+")
	patchwrite.write(patch_out)

	bug_out = gpt_infer(bug_prompt)
	print(f"BELIEFS ABOUT BUG FILE:\n {bug_out}")
	fullbugname = os.path.basename(bugFile)
	bug_name, _ = fullbugname.split(".")
	bugwrite = open(f"{RESULTS_DIR}/{bug_name}.txt", "w+")
	bugwrite.write(bug_out)
  
	bugfix_out = gpt_infer(bugfix_prompt)
	print(f"BELIEFS ABOUT BUG FIXED FILE:\n {bugfix_out}")
	fullbugfixname = os.path.basename(bugFixFile)
	bug_fix_name, _ = fullbugfixname.split(".")
	bugfixwrite = open(f"{RESULTS_DIR}/{bug_fix_name}.txt", "w+")
	bugfixwrite.write(bugfix_out)
	

def main(): 
	bugFile1 = "/home/sallyjunsongwang/DeviantBugs/bugs/linux/bug1.c"
	bugFile2 = "/home/sallyjunsongwang/DeviantBugs/bugs/linux/bug2.c"
	bugFile3 = "/home/sallyjunsongwang/DeviantBugs/bugs/linux/bug3.c"
	patchFile1 = "/home/sallyjunsongwang/DeviantBugs/bugs/linux/bug1_fullpatch.patch"
	patchFile2 = "/home/sallyjunsongwang/DeviantBugs/bugs/linux/bug2_fullpatch.patch"
	patchFile3 = "/home/sallyjunsongwang/DeviantBugs/bugs/linux/bug3_fullpatch.patch"
	bugFixFile1 = "/home/sallyjunsongwang/DeviantBugs/fixes/linux/bug1_fix.c"
	bugFixFile2 = "/home/sallyjunsongwang/DeviantBugs/fixes/linux/bug2_fix.c"
	bugFixFile3 = "/home/sallyjunsongwang/DeviantBugs/fixes/linux/bug3_fix.c"
	infer_beliefs(patchFile1, bugFile1, bugFixFile1)
	infer_beliefs(patchFile2, bugFile2, bugFixFile2)
	infer_beliefs(patchFile3, bugFile3, bugFixFile3)
	
if __name__ == "__main__":
	main()
	