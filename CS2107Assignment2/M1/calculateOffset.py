#address = input("Input leaked memory \n")
#address = int(address, 16)
#winAdd = hex(address-943)
#print(f"win() address is {winAdd}")

def calculateNewAdd(oldAdd):
	newAdd = hex(int(oldAdd, 16)-943)
	print (f"new address is {newAdd}")
	return newAdd
