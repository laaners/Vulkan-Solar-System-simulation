## GIT workflow
### 0) From main branch
From main branch, if branch_myname does not exist:
```bash
git pull
```

### 1) Create new branch
```bash
git branch branch_myname
```
```bash
git checkout branch_myname
```
### 2) Pull requests
In branch_myname do your stuff, once finished:
```bash
git add .
```
```bash
git commit -m "message"
```
```bash
git push
```
Then on github open a pull request and merge if there are no conflicts
## Merge changes to local branch
If branch_myname exists and someone else has made some changes, we switch to main
```bash
git checkout main
```
```bash
git pull
```
```bash
git checkout branch_myname
```
```bash
git merge main
```
Then we repeat step 2) and 3)

