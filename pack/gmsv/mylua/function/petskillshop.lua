--宠物学习技能时判断，返回1为可学习，返回0为不可学习
function FreePetSkillShop( talkerindex, petindex, oldSkillID, newSkillID)
	--print(talkerindex, petindex, oldSkillID, newSkillID);
	--技能限制码表
	local CodeTxt = petskill.getChar(newSkillID,"种类");--取得限制码
	local skillname = petskill.getChar(newSkillID,"名称");--取得技能名
	local Codeid = -1;--取得限制码
	local enemybaseid = pet.GetEnemyBaseId(petindex);--取得enemybaseid
	for i=1,table.getn(petskillcode) do
		if CodeTxt == petskillcode[i][1] then
			Codeid = i;
			break ;
		end
	end
	if Codeid > 0 then
		local CodeCf = false;--是否可学习
		for i=1,table.getn(petskillcode[Codeid][2]) do
			if enemybaseid == petskillcode[Codeid][2][i] then
				CodeCf = true;
				break;
			end
			--print(pet.getDataChar(petskillcode[Codeid][2][i], "名字"));
		end
		if CodeCf == true then
			return 1;
		else
			token = "[style c=6]["..char.getChar(petindex,"名字").."]无法学习！[/style]"
					.."\n[style c=4]技能["..skillname.."]可学习的宠物为：[/style]\n";
			local nametxt = {};
			for i=1,table.getn(petskillcode[Codeid][2]) do
				table.insert(nametxt,pet.getDataChar(petskillcode[Codeid][2][i], "名字"));
			end
			local han = table.getn(nametxt);
			for i=1,han do
				local toid1 = (i-1)*3+1;
				local toid2 = (i-1)*3+2;
				local toid3 = (i-1)*3+3;
				local myid =0;
				if nametxt[toid1] ~= nil then
					myid = myid+1;
				end
				
				if nametxt[toid2] ~= nil then
					myid = myid+1;
				end
				if nametxt[toid3] ~= nil then
					myid = myid+1;
				end
				if myid == 1 then
					token = token .. string.format( "%-18s\n",nametxt[toid1]);
				elseif myid == 2 then
					token = token .. string.format( "%-18s %-18s\n",nametxt[toid1],nametxt[toid2]);
				elseif myid == 3 then
					token = token .. string.format( "%-18s %-18s %-18s\n",nametxt[toid1],nametxt[toid2],nametxt[toid3]);
				end
			end
			char.Updata(talkerindex,"石币");
			lssproto.windows(talkerindex, 45, 8, -1, -1, token);
			return 0;
		end
	end
	
	return 1;
end

function mydata()
	petskillcode = {{"Aa",{725}}--捐献
					,{"Ab",{127,128,1065,1066,51,52,53,54,743,744,745,746}}--针刺外皮
					,{"Ac",{145,146}}--浴血狂狼
					,{"Ad",{933}}--火线猎杀
					,{"Ae",{148,149}}--媚惑术
					,{"Af",{51,52,53,54,743,744,745,746,1140}}--铁壁
					,{"Ag",{160}}--难得糊涂
					,{"Ah",{165,166,167,168}}--破除沉默
					,{"Ai",{768}}--破除防御之2
					,{"Aj",{777}}--浴血狂袭
					,{"Ak",{1140}}--地灵威能
					,{"Al",{170}}--乌力化
					,{"An",{1147}}--黑乌力化
					,{"Am",{176,177}}--逆转
					,{"Ao",{178,179}}--狂狮怒吼
					,{"Ap",{1180,1181}}--净化之舞
					,{"Aq",{1178,1179}}--群蜂乱舞
					,{"Ar",{1184,1185}}--蚁葬
					,{"As",{1178,1179}}--群蜂乱舞改
					,{"Ay",{1189,1190,1191,1192}}--肆无忌蛋
					,{"Ta",{304,203,968}}--T一击必杀改
					,{"Tb",{304,203,968}}--T背水之战之其３
					,{"Tc",{304,203,968}}--T不防守战法2
					,{"Td",{304,203,968}}--T不防守战法3
					,{"Te",{304,203,968}}--T四段攻击
					,{"Tf",{304,203,968}}--T五段攻击
					,{"Tg",{35,36,37,38}}--致命攻击
					,{"Ba",{957,958,959,960}}--嗜血技
					,{"Bd",{918}}--MP攻击
					,{"Be",{918}}--MP攻击2
					,{"Bl",{5020,5021,5022,5023,5024}}--愤怒
					,{"Bt",{3007}}--自然
					,{"Bu",{3008}}--凰岚
					,{"Br",{3014}}--涌泉
					,{"Bs",{3015}}--炙热
					,{"Bf5",{777}}--G一击必杀
					,{"Ca",{291,292,293,294,807,808,809,810,910}}--落马术
					,{"Dh",{3010,3011,3012,3013}}--大嘴袭击
					,{"Ga",{957,958,959,960}}--虚弱术
					,{"Ha",{969}}--闪避术
					,{"Ra",{969}}--沉默术
					,{"Ia",{981,982,983,984}}--暗月变身
					,{"Ja",{985}}--大地铠甲
					,{"Ka",{1047}}--水灵滋润
					,{"La",{986}}--火焰威能
					,{"Ma",{1048}}--风之吹息
					,{"Na",{1052,1053,1054,1055}}--怯战
					,{"Oa",{1060}}--魔之诅咒
					,{"Pa",{1133}}--栗子连激
					,{"Qa",{968}}--疾速攻击
					,{"Re",{1186,1187}}--憾甲一击
					,{"Sa",{301,302,303,811,979,812,813,814}}--撕裂伤口1
					,{"Sb",{811,979,812,813,814}}--撕裂伤口2
					,{"Se",{150}}--爆裂攻击
					,{"Sc",{814}}--撕裂伤口3
					,{"Wa",{979,169,965}}--狂暴攻击
					,{"Ua",{55,56}}--毒煞蔓延
					,{"Va",{90,96}}--音波冲击
					,{"Xa",{125,126,311,312,313,314,315,815,816,817,81,82,83,84,85,86,87,88,1109,1110,1097,1098}}--回旋攻击
					,{"Ya",{109,116}}--威吓攻击
					};
end

function reload(charaindex, data)
	print("重读宠技限制");
	mydata();
end

function main()
	mydata();
	magic.addLUAListFunction("重读宠技限制", "reload", "", 3, "[重读宠技限制]");
end

