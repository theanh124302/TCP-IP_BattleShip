
 username; password; position; accountId; opponent; status; sign; incorrect; findStatus; boardId

username; password; accountId; status; 
   


Mã trả về : Ý nghĩa

1.Login : 1user+pass
10: Khong tim duoc user
11: Login thanh cong
12: Tai khoan bi khoa do sai qua 3 lan
13: Sai pass (3 lan lien tuc ban se bi khoa)
14: Tai khoan da bi khoa
15: Tai khoan da duoc dang nhap tren thiet bi khac
cot3: 0: bi khoa, 1: binh thuong
cot4: 0: chua dnhap, 1: da dang nhap 
cot5: so lan nhap sai mat khau lien tiep

2.Logout : 2user
20: Dang xuat

3.SignUp : 3user+pass+cfpass
30: Dang ky thanh cong
31: User da ton tai
32: Cfpass khong hop le

4.ChangePass : 4user+pass+newpass
40: Doi pass thanh cong
41: Mat khau moi trung mat khau cu
42: Pass cũ sai


5.FindMatch : 5+accountId
50: Dang tim tran
51+opponentId: Da tim duoc tran

findStatus (cot 6): 0: chua tim tran, 1: dang tim tran, 2: da tim thay tran, 3: da san sang, 4: da vao tran


6.ExitFindMatch : 6+accountId
60: Thoat tran thanh cong
gui lai cho doi thu "85" (thong bao thang do doi thu thoat tran) neu dang trong tran hoac dang trong ready
neu thoat tran sau khi ket thuc game dau thi gui cho doi thu "84"



7: ready+user+pos+skinID
70 ben con lai chua ready , 7SkinID : 2 ben da ready (73,75,...)

8: atk+user+index
vi du : 8+a+32
trung manh cuoi cung (end): p1 : 83 (thang), p2 : 82 (thua)
trung nhung khong phai manh cuoi : p1 : 81, p2 : 832
truot : p1 : 80, p2: 832
truong hop khac : 84: doi thu out trong man replay
85: doi thu out khi dang ready, trong tran (-> win, quay tro lai man hinh chinh)


10: 10+user
tra ve 0+so tran win+so tran lose+score (0+2+3+150)


11: 11+user (Tao ban)
aBoardID (vi du : a1, a17,a40)

12: 12+user (Xem cac ban duoc nguoi khac tao de join)
tra ve danh sach id ban (vi du : b+3+5+11+12)

13: 13+user (Xem danh sach nguoi choi onl)
tra ve danh sach id (vi du : b+3+5+11+12)

14: 14+user+boardId (join ban)
tra ve ussername doi thu (d+username)
neu that bai tra ve (d+0)

15: 15+p1Name+p2Name+boardId (invite)
gui cho doi thu loi moi i+username(nguoi gui)+boardId

16: 16+p1Name+p2Name+boardId (kick)
gui ve "f1" cho p1, "f0" cho p2

18: 18+username+boardId (chap nhan loi moi)
thanhcong : gui cho nguoi gui loi moi user nguoi chap nhan (j+user), gui cho nguoi chap nhan la chap nhan thanh cong "j1" (chuyen sang man hinh "sanh/ban")
that bai: gui lai cho nguoi bam chap nhan la phong da day "j0"


17: 17+user+boardId (start ban -> ready)
tra ve username doi thu (5username)

19: 19+user (xem top 5 nguoi co diem cao nhat)
tra ve 5 nguoi choi co thu hang cao nhat: 
vi du : r+1+a+370+2+b+320+3+c+0
a xep top 1 voi 370 diem, b top 2 320 diem, c top 3 0 diem


20: 20+user1+user2+mess (chat)
neu u2 dang onl, nhan duoc h+u1+mess
neu u2 off, u1 se nhan lai h+0

21: 21+user+boardId (leave)
neu la p2, gui la "f1" cho p1,
neu la p1, gui la "f2" cho p2, doi cho p2 thanh p1 tren giao dien

***** Luu y nghiep vu khi tao ban: p1 tao ban, p2 join ban, p1 bam start => chuyen sang man hinh ready (bam start tuong duong vua match tran xong)
