clear;

% Player
player = 'pq';

% If the filename has some extension in the end
extension = '';

% Import data
filename = ['../statistics/stats_pert_' player extension '.csv'];
data = csvread(filename);

% Game parameters
N_perturb = data(1, 1);
perturb_max = data(1, 2);
N_games = data(1, 3);
N_rows = data(1, 4);
total_sticks = data(1, 5);

% Statistics
stats = data(2:end, 1);

% Display game parameters
fprintf(['\n' player(1) ' vs ' player(2) ':\n']);
fprintf(['  N_perturb     = ' num2str(N_perturb) '\n']);
fprintf(['  N_games       = ' num2str(N_games) '\n']);
fprintf(['  N_rows        = ' num2str(N_rows) '\n']);
fprintf(['  total_sticks  = ' num2str(total_sticks) '\n\n']);

% Data
N_vals = length(stats);
step = perturb_max/N_perturb;

% Plot
figure;
x_vals = 0:step:(perturb_max - step);
plot(x_vals, stats, '*r');
ylim([0 1]);
%ylim([(min(stats) - 0.1) (max(stats) + 0.1)]);
grid on;
title([player(1) ' vs ' player(2)]);
xlabel('r');
ylabel(['win rate (' player(2) '-player)']);


