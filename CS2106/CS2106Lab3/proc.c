int main( ) {
	int cid[9] = {0};
	int level = 1;
	for (int i = 0; i < 4; i++) {
		cid[i] = fork();
		printf("%d ", level);
		level +=1;
		for (int i = 0; i < 9; i++) {
			printf("%d ", cid[i]);
		}
		printf(" \n");
	}
	return 0;
}

