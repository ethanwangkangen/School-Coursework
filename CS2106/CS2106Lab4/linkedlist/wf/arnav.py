def passcode_i(s,seq):

    b=''

    for i in seq:

        if i=='+':

            s=(s+1)%10

            b+=str(s)

        else:

            s=(s-1)%10

            b+=str(s)

    return b

    

def passcode_r(s,seq):

    if not seq:

        return ''

    elif seq[0]=='+':

        s=(s+1)%10

        return str(s)+passcode_r(s,seq[1:])

    elif seq[0]=='-':

        s=(s-1)%10

        return str(s)+passcode_r(s,seq[1:])

def passcode_o(s,seq):

    return "".join(str(s:=(s + (1 if i=='+' else -1))%10) for i in seq)

print(passcode_i(2, '++++'))