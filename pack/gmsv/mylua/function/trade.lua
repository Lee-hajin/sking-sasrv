function getIntPart(x)
    if x <= 0 then
       return math.ceil(x);
    end

    if math.ceil(x) == x then
       x = math.ceil(x);
    else
       x = math.ceil(x) - 1;
    end
    return x;
end

function FreeTradePet( charaindex, petindex )
	return 0
end

function FreeTradeItem( charaindex, itemindex )
	if item.getInt(itemindex, "���") == 22001 then
		local data = item.getChar(itemindex, "�ֶ�")--��ȡitem6���ֶ�
		local valiity = other.atoi(data)
		if other.time() >= valiity then
			char.TalkToCli(charaindex, -1, "��������ʾ��֧Ʊ�ѹ��ڣ��޷����н��ס�", "��ɫ")
			return 1
		end
	end
	return 0
end

function data()

end

function main()
	data()
end

