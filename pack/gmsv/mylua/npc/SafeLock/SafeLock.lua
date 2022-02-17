function SafeTalked(talkerindex)
	token = "4\n               『账号安全管理员』"
				.."\n\n不要轻易永久解锁，不要为了方便损失安全！\n"
				.."\n                 永久解开安全锁"
				.."\n                 恢复正常安全锁";
	lssproto.windows(talkerindex, "选择框", "取消", 1, char.getWorkInt( npcindex, "对象"), token)
end

--NPC对话事件(NPC索引)
--function Talked(meindex, talkerindex , szMes, color )
--	SafeTalked(talkerindex)
--end

function Talked(meindex, talkerindex , szMes, color )
	if npc.isFaceToFace(meindex, talkerindex) == 1 then 
		SafeTalked(talkerindex)
	end
end

--NPC窗口事件(NPC索引)
function WindowTalked ( meindex, talkerindex, seqno, select, data)
	if seqno == 1 then
		num = other.atoi(data)
		if num == 1 then
			if char.getInt(talkerindex, "安全锁") == 0 then
				local rndnum = math.random(101,999);
				token = "                 『" .. char.getChar(meindex, "名字") .. "』\n"
						.."永久解锁安全提示："
						.."\n请注意，永久解锁后无论任何设备登陆此账号都不需要输入"
						.."\n安全码即可拿走任何物品，非常危险，不建议使用此功能！"
						.."\n不要为了图方便而造成不可挽回的后果。"
						.. "\n\n   一意孤行，确定解锁请输入确认码（[style c=5]" .. rndnum .. "[/style]）";
				char.setWorkInt(talkerindex,"NPC临时1",rndnum)
				lssproto.windows(talkerindex, "输入框", "确定|取消", 2, char.getWorkInt( meindex, "对象"), token)
				--char.setInt(talkerindex, "安全锁",-1)
			elseif char.getInt(talkerindex, "安全锁") == 2 then
				char.TalkToCli(talkerindex, -1, "【友情提示】您的人物处于永久解锁状态，很不安全，建议恢复安全锁！", 6);
			else
				char.TalkToCli(talkerindex, -1, "【友情提示】您当前状态尚未解锁，请解锁后再来找我哦！", 6);
				token = "                 『" .. char.getChar(meindex, "名字") .. "』\n\n"
							.."请输入你的安全密码！\n输入正确后进入解锁状态，并非永久解锁。\n如需永久解锁需要在解锁状态下再找我吧。";
				lssproto.windows(talkerindex, "输入框", "确定|取消", 530, -1, token)
			end
		elseif num == 2 then
			char.setInt(talkerindex, "安全锁", 1);
			token = "                 『" .. char.getChar(meindex, "名字") .. "』\n\n"
							.."已经成功为您的账号恢复安全锁。\n\n爱护账号，拒绝盗号。\n\n请不要把账号借给他人哟！"
		
			lssproto.windows(talkerindex, "对话框", "确定", -1, char.getWorkInt( meindex, "对象"), token)
		end
	elseif seqno == 2 then
		if select ~= 1 then
			return
		end
		if data == "" then
			char.TalkToCli(talkerindex, -1, "[温馨提示]确认码输入错误，请重新输入确认码，确认码为括号内的三位数字。", 4);
			return
		end
		num = other.atoi(data)
		if num < 101 or num > 999 then
			char.TalkToCli(talkerindex, -1, "[温馨提示]确认码输入错误，请重新输入确认码，确认码为括号内的三位数字。", 4);
			return
		end
		if num ~= char.getWorkInt(talkerindex,"NPC临时1") then
			char.TalkToCli(talkerindex, -1, "[温馨提示]确认码输入错误，请重新输入确认码，确认码为括号内的三位数字。", 4);
			return
		end
		if char.getInt(talkerindex, "安全锁") == 0 then
			char.setInt(talkerindex, "安全锁",2);
			char.TalkToCli(talkerindex, -1, "【友情提示】您已经永久解锁成功。", 4);
		end
	end
end

function reload()
	print("重读NPC信息",char.getChar(npcindex, "名字"));
	mydata();
	char.setChar(npcindex, "名字",npcdata[1]);
	char.setInt(npcindex, "图像号", npcdata[2]);
	char.setInt(npcindex, "原图像号", npcdata[2]);
	char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
	char.setInt(npcindex, "方向", npcdata[6]);
	char.ToAroundChar(npcindex);
end

function mydata()
	npcdata = {"帐号安全中心",51268,777,47,33,6};
end

function Create(name, metamo, floor, x, y, dir)
	npcindex = npc.CreateNpc(name, metamo, floor, x, y, dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
end

function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction("重读安全锁NPC", "reload", "", 1, "[重读安全锁NPC]");
end

