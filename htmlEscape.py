'''
Usa as informações obtidas em https://www.utf8-chartable.de/ para gerar uma tabela de substituição em C++
'''

srcStr = '''¡	c2 a1
¢	c2 a2
£	c2 a3
¤	c2 a4
¥	c2 a5
¦	c2 a6
§	c2 a7
¨	c2 a8
©	c2 a9
ª	c2 aa
«	c2 ab
¬	c2 ac
­	c2 ad
®	c2 ae
¯	c2 af
°	c2 b0
±	c2 b1
²	c2 b2
³	c2 b3
´	c2 b4
µ	c2 b5
¶	c2 b6
·	c2 b7
¸	c2 b8
¹	c2 b9
º	c2 ba
»	c2 bb
¼	c2 bc
½	c2 bd
¾	c2 be
¿	c2 bf
À	c3 80
Á	c3 81
Â	c3 82
Ã	c3 83
Ä	c3 84
Å	c3 85
Æ	c3 86
Ç	c3 87
È	c3 88
É	c3 89
Ê	c3 8a
Ë	c3 8b
Ì	c3 8c
Í	c3 8d
Î	c3 8e
Ï	c3 8f
Ð	c3 90
Ñ	c3 91
Ò	c3 92
Ó	c3 93
Ô	c3 94
Õ	c3 95
Ö	c3 96
×	c3 97
Ø	c3 98
Ù	c3 99
Ú	c3 9a
Û	c3 9b
Ü	c3 9c
Ý	c3 9d
Þ	c3 9e
ß	c3 9f
à	c3 a0
á	c3 a1
â	c3 a2
ã	c3 a3
ä	c3 a4
å	c3 a5
æ	c3 a6
ç	c3 a7
è	c3 a8
é	c3 a9
ê	c3 aa
ë	c3 ab
ì	c3 ac
í	c3 ad
î	c3 ae
ï	c3 af
ð	c3 b0
ñ	c3 b1
ò	c3 b2
ó	c3 b3
ô	c3 b4
õ	c3 b5
ö	c3 b6
÷	c3 b7
ø	c3 b8
ù	c3 b9
ú	c3 ba
û	c3 bb
ü	c3 bc
ý	c3 bd
þ	c3 be
ÿ	c3 bf'''

linhas = srcStr.split('\n');
s = '#define tamanhoTabelaSubs '+str(len(linhas))+'\nconst std::string tabelaSubs[tamanhoTabelaSubs][2] = {\n'

for linha in linhas:
    char = linha[0:1]
    subs = ('%'+linha[2:4]+'%'+linha[5:7]).upper()
    s+= '    {"'+subs+'","'+char+'"},\n'

s = s[:-2]
s+='\n};'
print(s)