function lua_string_split(str, split_char)
    local sub_str_tab = {};
    while (true) do
        local pos = string.find(str, split_char);
        if (not pos) then
            sub_str_tab[#sub_str_tab + 1] = str;
            break;
        end
        local sub_str = string.sub(str, 1, pos - 1);
        sub_str_tab[#sub_str_tab + 1] = sub_str;
        str = string.sub(str, pos + 1, #str);
    end

    return sub_str_tab;
end

--�桤��������
function RandItemList(itemlist)
	local TmItemList={};
	while table.getn(itemlist) > 0 do
		local n = math.random(1,table.getn(itemlist));
		if itemlist[n] ~= nil then
			table.insert(TmItemList,itemlist[n]);
			table.remove(itemlist,n);
		end
	end
	return TmItemList;
end

function ItemBoxCf(itemindex, charaindex, toindex, haveitemindex)
	local strdata = item.getChar(itemindex, "�ֶ�");
	local itemnum = char.GetEmptyItemNum(charaindex);--ȡ�õ���������
	local petnum = char.GetEmptyPetNum(charaindex);--ȡ�ó���������
	local BoxData = lua_string_split(strdata,"|");
	local BoxTime = item.getInt(itemindex,"HP");
	if table.getn(BoxData) <= 0 then
		char.TalkToCli(charaindex, -1, "����������⣡", 6);
		return ;
	end
	if BoxData[1] == "����" then
		local itemlist = {};
		if BoxTime == 20190130 then
			itemlist = {{{1,4503},{1,4502},{2,5030}}--ս����1 2 3 ��Ʒ
					   ,{{1,4503},{1,4502},{2,5030}}--�;���1 2 3 ��Ʒ
					   ,{{1,4503},{1,4502},{2,5030}}--������1 2 3 ��Ʒ
					   ,{{1,4503},{1,4502},{2,5030}}--������1 2 3 ��Ʒ
					   ,{{1,4503},{1,4502},{2,5030}}--������1 2 3 ��Ʒ
					   };
		end
		if table.getn(itemlist) <= 0 then
			char.TalkToCli(charaindex, -1, "�������δ���ţ�", 6);
			return ;
		end
		local BoxNum1 = other.atoi(BoxData[2]);
		local BoxNum2 = other.atoi(BoxData[3]);
		local GetName = "";
		if itemlist[BoxNum1][BoxNum2][1] == 1 then
			if itemnum < 1 then
				char.TalkToCli(charaindex, -1, "���ȿճ�1��������λ��", 6);
				return ;
			end
			local GetIndex = char.Additem( charaindex, itemlist[BoxNum1][BoxNum2][2]);
			GetName = item.getChar(GetIndex,"����");
			
		elseif itemlist[BoxNum1][BoxNum2][1] == 2 then
			if petnum < 1 then
				char.TalkToCli(charaindex, -1, "���ȿճ�1��������λ��", 6);
				return ;
			end
			local GetIndex = char.AddPet( charaindex, itemlist[BoxNum1][BoxNum2][2],1);
			GetName = char.getChar(GetIndex,"����");
		end
		char.TalkToCli(charaindex, -1, "��� ".. GetName .."��", 4);				
		char.DelItem(charaindex, haveitemindex);
	else
		char.TalkToCli(charaindex, -1, "�������δ���ţ�", 6);
		return ;
	end
	
end

function data()
end

function main()
	item.addLUAListFunction( "ITEM_BOXCF", "ItemBoxCf", "");
end
