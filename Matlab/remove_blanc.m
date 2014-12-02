function [ son_sortie ] = remove_blanc( son )

% son_sortie = abs(son) > max(abs(son))/3;
son_sortie = conv(abs(son),ones(1,100)) > 3;
a = find(son_sortie,1,'first');
b = find(son_sortie,1,'last');
son_sortie = son(a:b);

end

