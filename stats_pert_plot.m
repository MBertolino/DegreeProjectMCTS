clear;

% Player
player = 'sx';

% If the filename has some extension in the end
extension = '_c100_a';

% Import data
filename = ['../statistics/stats_pert_' player extension '.csv'];
data = csvread(filename);

% Game parameters
N_perturb = data(1, 1);
perturb_max = data(1, 2);
N_games = data(1, 3);
N_rows = data(1, 4);
rows = zeros(N_rows, 1);
for k = 1:N_rows
    rows(k) = data(1, 4+k);
end

% Statistics
stats = data(2:end, 1);

% Display game parameters
fprintf(['\n' player(1) ' vs ' player(2) ':\n']);
fprintf(['  N_perturb = ' num2str(N_perturb) '\n']);
fprintf(['  N_games = ' num2str(N_games) '\n']);
fprintf(['  N_rows  = ' num2str(N_rows) '\n\n']);
fprintf('Initial board:\n');
for i = 1:N_rows
    fprintf('  ');
    for j = 1:rows(i)
        fprintf('|');
    end
    fprintf('\n');
end
fprintf('\n');

% Data
N_vals = length(stats);
step = perturb_max/N_perturb;

% Plot
figure;
x_vals = 0:step:(perturb_max - step);
plot(x_vals, stats, '*b');
ylim([(min(stats) - 0.1) (max(stats) + 0.1)]);
grid on;
title([player(1) ' vs ' player(2)]);
xlabel('r');
ylabel(['win rate (' player(2) '-player)']);


